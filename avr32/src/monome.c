/*
  monome.c
  aleph-avr32

  monome device interface definitions

*/

// asf
#include "delay.h"
#include "print_funcs.h"
#include <string.h>
// aleph
#include "events.h"
#include "event_types.h"
#include "ftdi.h"
#include "monome.h"


//------ defines

// manufacturer string length
#define MONOME_MANSTR_LEN 6
// product string length
#define MONOME_PRODSTR_LEN 8
// serial string length
#define MONOME_SERSTR_LEN 9

//------- typedefs

//--- descriptor types

// protocol enumeration
typedef enum {
  eProtocol40h,      /// 40h and arduinome protocol (pre-2007)
  eProtocolSeries,   /// series protocol (2007-2011)
  eProtocolMext,     /// extended protocol (2011 - ? ), arcs + grids  
  eProtocolNumProtocols // dummy and count
} eMonomeProtocol;

// device enumeration
typedef enum {
  eDeviceGrid,   /// any grid device
  eDeviceArc,     /// any arc device
  eDeviceNumDevices // dummy and count
} eMonomeDevice;

// device descriptor
typedef struct e_monomeDesc {
  eMonomeProtocol protocol;
  eMonomeDevice device;
  u8 cols;  // number of columns
  u8 rows;  // number of rows
  u8 encs; // number of encoders
  u8 tilt;  // has tilt (??)  
} monomeDesc;


//-------------------------------------
//------ extern variables

// connected flag
u8 monomeConnect = 0;
// dirty flags for each quadrant or knob (bitwise)
u8 monomeFrameDirty = 0;
// a buffer big enough to hold all l data for 256 or arc4
// each led gets a full byte
u8 monomeLedBuffer[MONOME_MAX_LED_BYTES];

// global pointers to send functions.
read_serial_t monome_read_serial;
grid_led_t monome_grid_led;
grid_map_t monome_grid_map;
grid_level_map_t monome_grid_level_map;
ring_map_t monome_ring_map;

//-----------------------------------------
//----- static variables

// descriptor for connected device
static monomeDesc mdesc = {
  .protocol = eProtocolNumProtocols, // dummy
  .device = eDeviceNumDevices, // dummy
  .cols = 0,
  .rows = 0,
  .encs = 0,
  .tilt = 0,
};

// rx buffer
static  volatile u8* rxBuf;
// event data
static event_t ev;

//---------------------------------------------
//------ static function declarations

// setup for each protocol
static void setup_40h(u8 cols, u8 rows);
static void setup_series(u8 cols, u8 rows);
static u8 setup_mext(void);

// rx for each protocol
static void read_serial_40h(void);
static void read_serial_series(void);
static void read_serial_mext(void);

// tx for each protocol
static void grid_led_40h(u8 x, u8 y, u8 val);
static void grid_led_series(u8 x, u8 y, u8 val);
static void grid_led_mext(u8 x, u8 y, u8 val);

static void grid_map_40h(u8 x, u8 y, const u8* data);
static void grid_map_series(u8 x, u8 y, const u8* data);
static void grid_map_mext(u8 x, u8 y, const u8* data);

//static void grid_map_level_40h(u8 x, u8 val);
//static void grid_map_level_series(u8 x, u8 y, u8* data);
static void grid_map_level_mext(u8 x, u8 y, const u8* data);

//static void ring_set_mext(u8 n, u8 rho, u8 val);
static void ring_map_mext(u8 n, u8* data);

// event write
static inline void monome_grid_key_write_event( u8 x, u8 y, u8 val);
static inline void monome_grid_adc_write_event( u8 n, u16 val);
static inline void monome_ring_enc_write_event( u8 n, u8 val);
static inline void monome_ring_key_write_event( u8 n, u8 val);

//---------------------------------
//----- static variables

//  function pointer arrays
static read_serial_t readSerialFuncs[eProtocolNumProtocols] = {
  &read_serial_40h,
  &read_serial_series,
  &read_serial_mext,
};

static grid_led_t gridLedFuncs[eProtocolNumProtocols] = {
  &grid_led_40h,
  &grid_led_series,
  &grid_led_mext,
};

static grid_map_t gridMapFuncs[eProtocolNumProtocols] = {
  &grid_map_40h,
  &grid_map_series,
  &grid_map_mext,
};

/// TODO
/* static grid_level_map_t gridMapLevelFuncs[eProtocolNumProtocols] = { */
/*   NULL, // unsupported */
/*   NULL, // unsupported */
/*   &grid_map_level_mext, */
/* }; */

static ring_map_t ringMapFuncs[eProtocolNumProtocols] = {
  NULL, // unsupported
  NULL, // unsupported
  &ring_map_mext,
};

//================================================
//----- extern function definitions

// init
void init_monome(void) {
  // ftdi precedes each packet with 0x31 0x60. 
  // this is to avoid actually stripping these 2 bytes after each read
  rxBuf = ftdiRxBuf + FTDI_RX_BUF_OFFSET;
  u32 i;
  for(i=0; i<MONOME_MAX_LED_BYTES; i++) {
    monomeLedBuffer[i] = 0;
  }
  //  print_dbg("\r\n finished monome class init");
}

// determine if FTDI string descriptors match monome device pattern
u8 check_monome_device_desc(char* mstr, char* pstr, char* sstr) {
  char buf[16];
  u8 matchMan = 0;
  u8 i;
  //-- source strings are unicode so we need to look at every other byte
  // manufacturer
  for(i=0; i<MONOME_MANSTR_LEN; i++) {
    buf[i] = mstr[i*2];
  }
  buf[i] = 0;
  matchMan = ( strncmp(buf, "monome", MONOME_MANSTR_LEN) == 0 );
  /* print_dbg("\r\n manstring: "); */
  /* print_dbg(buf); */
 
  // serial number string
  for(i=0; i<MONOME_SERSTR_LEN; i++) {
    buf[i] = sstr[i*2];
  }
  buf[i] = 0;
  /* print_dbg("\r\n serial string: "); */
  /* print_dbg(buf); */
  if(matchMan == 0) {
    // didn't match the manufacturer string, but check the serial for DIYs
    if( strncmp(buf, "a40h", 4) == 0) {
      // this is probably an arduinome      
      mdesc.protocol = eProtocol40h;
      mdesc.device = eDeviceGrid;
      mdesc.cols = 8;
      mdesc.rows = 8;
      // tilt?
      return 1;
    } else {
      // not a monome
      return 0;
    }
  } else { // matched manufctrr string
    if(buf[0] != 'm') {
      // not a monome, somehow. shouldn't happen
      return 0;
    }
    if(buf[3] == 'h') {
      // this is a 40h
      setup_40h(8, 8);
      return 1;
    }
    if( strncmp(buf, "m64-", 4) == 0 ) {
      // series 64
      setup_series(8, 8);
      return 1;
    }
    if( strncmp(buf, "m128-", 5) == 0 ) {
      // series 128
      setup_series(8, 16);
      return 1;
    }
    if( strncmp(buf, "m128-", 5) == 0 ) {
      // series 256
      setup_series(16, 16);
      return 1;
    }
    // if we got here, serial number didn't match series or 40h patterns.
    // so this is probably an extended-protocol device.
    // we need to query for device attributes
    return setup_mext();
  }
}

// check dirty flags and refresh leds
void monome_grid_refresh(void) {
  print_dbg("\r\n monome grid refresh; framedirty flag: 0x");
  print_dbg_hex(monomeFrameDirty);

  if( monomeFrameDirty & 0b0001 ) {
    print_dbg("\r\n refreshing monome quadrant 0");
    grid_map_mext(0, 0, monomeLedBuffer);
    monomeFrameDirty &= 0b1110;
  }

  if( monomeFrameDirty & 0xb0010 ) {
    print_dbg("\r\n refreshing monome quadrant 1");
    if ( mdesc.cols > 8 ) {
      grid_map_mext(8, 0, monomeLedBuffer + 8);
      monomeFrameDirty &= 0b1101;
    }
  }

  if( monomeFrameDirty &  0b0100 ) { 
    print_dbg("\r\n refreshing monome quadrant 2");
    if( mdesc.rows > 8 ) {
      grid_map_mext(0, 8, monomeLedBuffer +  128);
      monomeFrameDirty &= 0b1011;
    }
  }

  if( monomeFrameDirty & 0b1000 ) {
    print_dbg("\r\n refreshing monome quadrant 3");
    if( (mdesc.rows > 8) && (mdesc.cols > 8) )  {
      grid_map_mext(8, 8, monomeLedBuffer + 136);
      monomeFrameDirty &= 0b0111;
    }
  }
}

//---- convert to/from event data

// grid key
static inline void monome_grid_key_write_event(u8 x, u8 y, u8 val) {
  u8* data = (u8*)(&(ev.eventData));
  data[0] = x;
  data[1] = y;
  data[2] = val;

  ev.eventType = kEventMonomeGridKey;
  post_event(&ev);

}
void monome_grid_key_parse_event_data(u32 data, u8* x, u8* y, u8* val) {
  u8* bdata = (u8*)(&data);
  *x = bdata[0];
  *y = bdata[1];
  *val = bdata[2];
}

// grid tilt / adc
static inline void monome_grid_adc_write_event( u8 n, u16 val) {
  // TODO
}
void monome_grid_adc_parse_event_data(u32 data, u8* n, u16* val) {
  // TODO
}

// ring encoder
static inline void monome_ring_enc_write_event( u8 n, u8 val) {
  // TODO
}
void monome_ring_enc_parse_event_data(u32 data, u8* n, s8* val) {
  /// TODO
}

// ring press/lift
static inline void monome_ring_key_write_event( u8 n, u8 val) {
  // TODO
}
void monome_ring_key_parse_event_data(u32 data, u8* n, u8* val) {
  // TODO
}

// set quadrant refresh flag from pos
void monome_calc_quadrant_flag(u8 x, u8 y) {
  if(x > 7) {
    if (y > 7) {      
      monomeFrameDirty |= 0b1000;
    }
    else {
      monomeFrameDirty |= 0b0010;
    }
  } else {
    if (y > 7) {
      monomeFrameDirty |= 0b0100;
    }
    else {
      monomeFrameDirty |= 0b0001;
    }
  } 

  print_dbg("\r\n monome_calc_quadrant_flag: 0x");
  print_dbg_hex(monomeFrameDirty);
}

// convert flat framebuffer idx to x,y
void monome_idx_xy(u32 idx, u8* x, u8* y) {
  *x = idx & 0xf;
  *y = (idx >> 4);
}

// convert x,y to framebuffer idx
u32 monome_xy_idx(u8 x, u8 y) {
  return x | (y << 4);
}



//=============================================
//------ static function definitions

// set function pointers
static inline void set_funcs(void) {
  monome_read_serial = readSerialFuncs[mdesc.protocol];
  monome_grid_led = gridLedFuncs[mdesc.protocol];
  monome_grid_level_map = gridMapFuncs[mdesc.protocol];
  monome_ring_map = ringMapFuncs[mdesc.protocol];
}

// setup 40h-protocol device
static void setup_40h(u8 cols, u8 rows) {
  mdesc.protocol = eProtocol40h;
  mdesc.device = eDeviceGrid;
  mdesc.cols = 8;
  mdesc.rows = 8;
  set_funcs();
  monomeConnect = 1;
}

// setup series device
static void setup_series(u8 cols, u8 rows) {
  print_dbg("\r\n setup series device");
  mdesc.protocol = eProtocolSeries;
  mdesc.device = eDeviceGrid;
  mdesc.cols = 8;
  mdesc.rows = 8;
  mdesc.tilt = 1;
  set_funcs();
  monomeConnect = 1;
}

// setup extended device, return success /failure of query
static u8 setup_mext(void) {
  u8 b0, b1, b2;
  u8 nb;
  u8 w = 0;
  mdesc.protocol = eProtocolMext;

  delay_ms(1);
  ftdi_write(&w, 1);	// query

  delay_ms(1);
  nb = ftdi_read();

  if(nb != 6 ){
    return 0;
  }

  print_dbg("\r\n local rx pointer: 0x");
  print_dbg_hex((u32)rxBuf);
  print_dbg("\r\n global ftdi rx pointer: 0x");
  print_dbg_hex((u32)ftdiRxBuf);

  b0 = (u8)rxBuf[0];
  b1 = (u8)rxBuf[1];
  b2 = (u8)rxBuf[2];

  if(b1 == 1) {
    mdesc.device = eDeviceGrid;
	 
    if(b2 == 1) {
      print_dbg("\r\n monome 64");
      mdesc.rows = 8;
      mdesc.cols = 8;
    }
    else if(b2 == 2) {
      print_dbg("\r\n monome 128");
      mdesc.rows = 8;
      mdesc.cols = 16;
    }
    else if(b2 == 4) {
      print_dbg("\r\n monome 256");
      mdesc.rows = 16; 
      mdesc.cols = 16;
    }
    else {
      return 0;
    }		
    mdesc.tilt = 1;
  }
  else if(b1 == 5) {
    mdesc.device = eDeviceArc;
    mdesc.encs = b2;
  } else {
    return 0;
  }
  set_funcs();
  monomeConnect = 1;
  print_dbg("\r\n connected monome device, mext protocol");
  return 1;
}

//----- protocol-specific functions

//--- rx
// rx for each protocol
static void read_serial_40h(void) {
}

static void read_serial_series(void) {
  u8 i;
  u8 b0, b1;
  u8 x=0;
  u8 y=0;
  u8 z=0;

  u8 com;
  u8 nb;
  // read from ftdi and get number of bytes.
  // data is at beginning of rxBuf
  nb = ftdi_read();
  if(nb) {
    com = *rxBuf;
    switch(com) {
    }
    for(i=0; i<nb; i+= 2) {
      b0 = (u8)rxBuf[i];
      b1 = (u8)rxBuf[i+1];
      x = (b1 & 0xf0) >> 4;
      y = b1 & 0xf;
      z = !((b0 & 0xf0) >> 4);
      monome_grid_key_write_event(x, y, z);
    }
  }
}

static void read_serial_mext(void) {
  static u8 nbr; // number of bytes read
  static u8 nbp; // number of bytes processed
  static u8* prx; // pointer to rx buf
  static u8 com;

  // read from ftdi and get number of bytes.
  // the data is at beginning of rxBuf  
  nbr = ftdi_read();
  
  if(nbr) {
    nbp = 0;
    prx = (u8*)rxBuf;
    while(nbp < nbr) {
      com = (u8)(*(prx++));    
      nbp++;
      /* print_dbg("\r\n read serial, mext protocol, data: 0x"); */
      /* print_dbg_hex(rxBuf[0]); */
      /* print_dbg(" ,  0x"); */
      /* print_dbg_hex(rxBuf[1]); */
      /* print_dbg(" ,  0x"); */
      /* print_dbg_hex(rxBuf[2]); */
      if(com == 0x20) { // grid key up
	monome_grid_key_write_event( *prx, *(prx+1), 0);
	nbp += 2;
	prx += 2;
      }
      else if (com == 0x21) { // grid key down
	monome_grid_key_write_event( *prx, *(prx+1), 1);
	nbp += 2;
	prx += 2;
      }
      else if (com == 0x50) { // ring delta
	monome_ring_enc_write_event( *prx, *(prx+1));
	nbp += 2;
	prx += 2;
      }
      else if (com == 0x51) { // ring key up
	monome_ring_key_write_event( *prx++, 0);
	prx++;
      }
      else if (com == 0x52) { // ring key down
	monome_ring_key_write_event( *prx++, 1);
	nbp++;
      }
      // ... other commands?
    }	
  }
}

//--- tx
static void grid_led_40h(u8 x, u8 y, u8 val) {
  // TODO
}

static void grid_led_series(u8 x, u8 y, u8 val) {
  static u8 tx[2];
  tx[0] = 0x20 & ((val > 0) << 4);
  tx[1] = (x << 4) | y;
  ftdi_write(tx, 2);
}

static void grid_led_mext(u8 x, u8 y, u8 val) {
  static u8 tx[3];
  tx[0] = 0x10 | (val > 0);
  tx[1] = x;
  tx[2] = y;
  ftdi_write(tx, 3);
}

// update a whole frame
// . note that our input data is one byte per led!!
// this will hopefully help optimize operator routines,
// which cannot be called less often than refresh/tx, and are therefore prioritized.
static void grid_map_mext( u8 x, u8 y, const u8* data ) {
  static u8 tx[11] = { 0x14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  //  static u8* pdata;
  static u8* ptx;
  static u8 i, j;
  tx[1] = x;
  tx[2] = y;
  
  //  pdata = data;
  ptx = tx + 3;
  
  // copy and convert
  for(i=0; i<MONOME_QUAD_LEDS; i++) {
    *ptx = 0;
    for(j=0; j<MONOME_QUAD_LEDS; j++) {
      // binary value of data byte to bitfield of tx byte
      *ptx |= ((*data > 0) << j);
      data++;
    }
    data += MONOME_QUAD_LEDS; // skip the rest of the row to get back in target quad
    ptx++;
  }
  ftdi_write(tx, 11);
}


static void grid_map_40h(u8 x, u8 y, const u8* data) {
  // TODO : (use 8 row commands and ignore x/y)
}

static void grid_map_series(u8 x, u8 y, const u8* data) {
  // TODO
}

static void grid_map_level_mext(u8 x, u8 y, const u8* data) {
  // TODO
}

static void ring_map_mext(u8 n, u8* data) {
  // TODO
}














