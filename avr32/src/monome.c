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

static void grid_map_40h(u8 x, u8 y, u8* data);
static void grid_map_series(u8 x, u8 y, u8* data);
static void grid_map_mext(u8 x, u8 y, u8* data);

//static void grid_map_level_40h(u8 x, u8 val);
//static void grid_map_level_series(u8 x, u8 y, u8* data);
static void grid_map_level_mext(u8 x, u8 y, u8* data);

//static void ring_set_mext(u8 n, u8 rho, u8 val);
static void ring_map_mext(u8 n, u8* data);

// event write
static inline void monome_grid_key_write_event(event_t* ev, u8 x, u8 y, u8 val);
static inline void monome_grid_adc_write_event(event_t* ev, u8 n, u16 val);
static inline void monome_ring_enc_write_event(event_t* ev, u8 n, u8 val);
static inline void monome_ring_key_write_event(event_t* ev, u8 n, u8 val);

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
extern void init_monome(void) {
  // ftdi precedes each packet with 0x31 0x60. 
  // this is to avoid actually stripping these 2 bytes after each read
  rxBuf = ftdiRxBuf + FTDI_RX_BUF_OFFSET;
  print_dbg("\r\n finished monome class init");
}

// determine if FTDI string descriptors match monome device pattern
extern u8 check_monome_device(char* mstr, char* pstr, char* sstr) {
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
  print_dbg("\r\n manstring: ");
  print_dbg(buf);
 
  // serial number string
  for(i=0; i<MONOME_SERSTR_LEN; i++) {
    buf[i] = sstr[i*2];
  }
  buf[i] = 0;
  print_dbg("\r\n serial string: ");
  print_dbg(buf);
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

/* extern void monome_read_serial(void) { */
/*   (*readSerialFunc)(); */
/* } */

/* extern void monome_grid_led(u8 x, u8 y, u8 val) { */
/*   (*gridLedFunc)(x, y, val); */
/* } */

/* extern void monome_grid_map(u8 x, u8 val) { */
/*   (*gridMapFunc)(x, val); */
/* } */

/* extern void monome_grid_map_level(u8 y, u8 val) { */
/*   (*gridLevelMapFunc)(y, val); */
/* } */

/* extern void monome_arc_led(u8 n, u8 idx, u8 val) { */
/*   (*arcLedFunc)(n, idx, val); */
/* } */

//---- convert to/from event data

// grid key
static inline void monome_grid_key_write_event(event_t* ev, u8 x, u8 y, u8 val) {
  u8* data = (u8*)(&(ev->eventData));
  data[0] = x;
  data[1] = y;
  data[2] = val;
  ev->eventType = kEventMonomeGridKey;
}
extern void monome_grid_key_read_event(event_t* ev, u8* x, u8* y, u8* val) {
  u8* data = (u8*)(&(ev->eventData));
  *x = data[0];
  *y = data[1];
  *val = data[2];
  ev->eventType = kEventMonomeGridKey;
}

// grid tilt / adc
static inline void monome_grid_adc_write_event(event_t* ev, u8 n, u16 val) {
  // TODO
}
extern void monome_grid_adc_read_event(event_t* ev, u8* n, u16* val) {
  // TODO
}

// ring encoder
static inline void monome_ring_enc_write_event(event_t* ev, u8 n, u8 val) {
  // TODO
}
extern void monome_ring_enc_read_event(event_t* ev, u8* n, s8* val) {
  /// TODO
}

// ring press/lift
static inline void monome_ring_key_write_event(event_t* ev, u8 n, u8 val) {
  // TODO
}
extern void monome_ring_key_read_event(event_t* ev, u8* n, u8* val) {
  // TODO
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
      mdesc.rows = 8;
      mdesc.cols = 8;
    }
    else if(b2 == 2) {
      mdesc.rows = 8;
      mdesc.cols = 16;
    }
    else if(b2 == 4) {
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
      monome_grid_key_write_event(&ev, x, y, z);
      post_event(&ev);
    
    }
  }
}

static void read_serial_mext(void) {
  static u8 nb;
  static u8 com;

  // read from ftdi and get number of bytes.
  // the data is at beginning of rxBuf  
  nb = ftdi_read();
  
  if(nb) {
    com = (u8)(*rxBuf);    
    /* print_dbg("\r\n read serial, mext protocol, data: 0x"); */
    /* print_dbg_hex(rxBuf[0]); */
    /* print_dbg(" ,  0x"); */
    /* print_dbg_hex(rxBuf[1]); */
    /* print_dbg(" ,  0x"); */
    /* print_dbg_hex(rxBuf[2]); */
    if(com == 0x20) { // grid key up
      monome_grid_key_write_event(&ev, rxBuf[1], rxBuf[2], 0);
      post_event(&ev);
    }
    else if (com == 0x21) { // grid key down
      monome_grid_key_write_event(&ev, rxBuf[1], rxBuf[2], 1);
      post_event(&ev);
    }
    else if (com == 0x50) { // ring delta
      monome_ring_enc_write_event(&ev, rxBuf[1], rxBuf[2]);
      post_event(&ev);
    }
    else if (com == 0x51) { // ring key up
      monome_ring_key_write_event(&ev, rxBuf[1], 0);
      post_event(&ev);
    }
    else if (com == 0x52) { // ring key down
      monome_ring_key_write_event(&ev, rxBuf[1], 1);
      post_event(&ev);
    }
    // ... other commands?
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
// note that our input data is one byte per led!!
// this will hopefully help optimize operator routines,
// which cannot be called less often than refresh/tx, and are therefore prioritized.
static void grid_map_mext( u8 x, u8 y, u8* data ) {
  static u8 tx[11] = { 0x14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  u8* pdata;
  u8* ptx;
  u8 i, j;
  tx[1] = x;
  tx[2] = y;
  
  pdata = data;
  ptx = tx + 3;
  
  // copy and convert
  for(i=0; i<8; i++) {
    *ptx = 0;
    for(j=0; j<8; j++) {
      *ptx |= ((*pdata > 0) << i);
      pdata++;
    }
    ptx++;
  }
  ptx++;

  ftdi_write(tx, 11);
}


static void grid_map_40h(u8 x, u8 y, u8* data) {
  // TODO : (use 8 row commands and ignore x/y)
}

static void grid_map_series(u8 x, u8 y, u8* data) {
  // TODO
}

static void grid_map_level_mext(u8 x, u8 y, u8* data) {
  // TODO
}

static void ring_map_mext(u8 n, u8* data) {
  // TODO
}

















/* 

EXTENDED (current): 
updated 110626 (corrected some typos)



monome protocol / OSC spec
--------------------------

//// input (to serialosc/device)

// sys
note:		no need to re-query the device. serialosc can query on connect then store results?

/sys/prefix
/sys/port
/sys/host

pattern:	/sys/query
desc:		request device information
serial:		[0x00]

pattern:	/sys/id
desc:		request device ID string
serial:		[0x01]

pattern:	/sys/size	
desc:		request grid size
serial:		[0x05]

// led-grid

pattern:	/prefix/led/set x y s	
desc:		set led state
args:		x = x value (0-255)
			y = y value (0-255)
			s = on/off (0-1)
serial:		if s = 0, [0x10, x, y]
			if s = 1, [0x11, x, y]
			
pattern:	/prefix/led/all s	
desc:		set all leds
args:		s = on/off (0-1)
serial:		if s = 0, [0x12]
			if s = 1, [0x13]

pattern:	/prefix/led/map x y d[8]
desc:		set 8x8 block of leds, with offset
args:		x = x offset, will be floored to multiple of 8 by firmware
			y = y offset, will be floored to multiple of 8 by firmware
			d[8] = bitwise display data
serial:		[0x14, x, y, d[8]]

pattern:	/prefix/led/row x y d
desc:		set 8x1 block of leds, with offset
args:		x = x offset, will be floored to multiple of 8 by firmware
			y = y offset
			d = bitwise display data
serial:		[0x15, x, y, d]
note:		d can be an array. for each additional element, another serial packet is sent, with x offset incremented by 8.

pattern:	/prefix/led/col x y d
desc:		set 1x8 block of leds, with offset
args:		x = x offset
			y = y offset, will be floored to multiple of 8 by firmware
			d = bitwise display data
serial:		[0x16 x, y, d]
note:		d can be an array. for each additional element, another serial packet is sent, with y offset incremented by 8.

pattern:	/prefix/led/intensity i
desc:		set intensity for entire grid
args:		i = intensity (0-15)
serial:		[0x17, i]

pattern:	/prefix/led/level/set x y i	
desc:		set led intensity
args:		x = x value (0-255)
			y = y value (0-255)
			i = intensity (0-15)
serial:		[0x18, x, y, i]

pattern:	/prefix/led/level/all s	
desc:		set all leds to level
args:		i = intensity (0-15)
serial:		[0x19, i]

pattern:	/prefix/led/level/map x y d[64]
desc:		set 8x8 block of leds levels, with offset
args:		x = x offset, will be floored to multiple of 8 by firmware
		y = y offset, will be floored to multiple of 8 by firmware
		d[64] = intensities
serial:		[0x1A, x, y, d[32]]

pattern:	/prefix/led/level/row x y d[8]
desc:		set 8x1 block of led levels, with offset
args:		x = x offset, will be floored to multiple of 8 by firmware
		y = y offset
		d[8] = intensities
serial:		[0x1B, x, y, d[4]]
note:		if d[4] has more than 8 elements (multiple of 8) send multiple serial packets with proper offset

pattern:	/prefix/led/level/col x y d[8]
desc:		set 1x8 block of led levels, with offset
args:		x = x offset
		y = y offset, will be floored to multiple of 8 by firmware
		d[8] = intensities
serial:		[0x1C x, y, d[4]]
note:		if d[4] has more than 8 elements (multiple of 8) send multiple serial packets with proper offset


// led-ring

pattern:	/prefix/ring/set n x a
desc:		set led x of ring n to value a
args:		n = ring number
			x = led number
			a = value (0-15)
serial:		[0x90, n, x, a]

pattern:	/prefix/ring/all n a
desc:		set all leds of ring n to a
args:		n = ring number
			a = value
serial:		[0x91, n, a]

pattern:	/prefix/ring/map n d[32]
desc:		set leds of ring n to array d
args:		n = ring number
			d[32] = 64 states, 4 bit values, in 32 consecutive bytes
			d[0] (0:3) value 0
			d[0] (4:7) value 1
			d[1] (0:3) value 2
			....
			d[31] (0:3) value 62
			d[31] (4:7) value 63
serial:		[0x92, n d[32]]

pattern:	/prefix/ring/range n x1 x2 a
desc:		set leds inclusive from x1 and x2 of ring n to a
args:		n = ring number
			x1 = starting position
			x2 = ending position
			a = value
serial:		[0x93, n, x1, x2, a]
note:		set range x1-x2 (inclusive) to a. wrapping supported, ie. set range 60,4 would set values 60,61,62,63,0,1,2,3,4. always positive direction sweep. ie. 4,10 = 4,5,6,7,8,9,10 whereas 10,4 = 10,11,12,13...63,0,1,2,3,4 



//// output (from serialosc/device)

// sys

pattern:	/sys/query s n
desc:		device information
serial:		[0x00, s, n]
args:		s = subsection, enumerated as strings [null, "led-grid", "key-grid", "digital-out", "digital-in", "encoder", "analog-in", "analog-out", "tilt", "led-ring"]
			n = number

pattern:	/sys/id s[64]
desc:		device ID string
serial:		[0x01, s[64]]
args:		s = id string, 64 chars

pattern:	/sys/size x y	
desc:		grid size
serial:		[0x05, x, y]
args:		x = x size
			y = y size
			
// key

pattern:	/prefix/key x y s
desc:		key state change
serial:		if s = 0 [0x00, x, y] key up
			if s = 1 [0x01, x, y] key down
args:		x = x position
			y = y position
			
// encoder

pattern:	/prefix/enc/delta n d
desc:		encoder position change
serial:		[0x50, n, d]
args:		n = encoder number
			d = delta (signed char)

pattern:	/prefix/enc/key n d
desc:		encoder position change
serial:		if d = 0 [0x51 n] key up
			if d = 1 [0x52 n] key down
args:		n = encoder number



*/


/*

SERIES:
monome serial protocol series 256/128/64
brian crabtree - tehn@monome.org

revision: 070903


from device:

message id:	(0) keydown
bytes:		2
format:		iiii.... xxxxyyyy
			i (message id) = 0
			x (x value) = 0-15 (four bits)
			y (y value) = 0-15 (four bits)
decode:		id match: byte 0 >> 4 == 0
			x: byte 1 >> 4
			y: byte 1 & 0x0f


message id:	(1) keyup
bytes:		2
format:		iiii.... xxxxyyyy
			i (message id) = 1
			x (x value) = 0-15 (four bits)
			y (y value) = 0-15 (four bits)
decode:		id match: byte 0 >> 4 == 1
			x: byte 1 >> 4
			y: byte 1 & 0x0f
			
			
			
to device:

message id:	(2) led_on
bytes:		2
format:		iiii.... xxxxyyyy
			i (message id) = 2
			x (x value) = 0-15 (four bits)
			y (y value) = 0-15 (four bits)
encode:		byte 0 = (id) << 4 = 32
			byte 1 = (x << 4) | y
			

message id:	(3) led_off
bytes:		2
format:		iiii.... xxxxyyyy
			i (message id) = 3
			x (x value) = 0-15 (four bits)
			y (y value) = 0-15 (four bits)
encode:		byte 0 = (id) << 4 = 48
			byte 1 = (x << 4) | y


message id:	(4) led_row1
bytes:		2
format:		iiiiyyyy aaaaaaaa
			i (message id) = 4
			y (row to update) = 0-15 (4 bits)
			a (row data 0-7) = 0-255 (8 bits)
encode:		byte 0 = ((id) << 4) | y = 64 + y
			byte 1 = a


message id:	(5) led_col1
bytes:		2
format:		iiiixxxx aaaaaaaa
			i (message id) = 5
			x (col to update) = 0-15 (4 bits)
			a (col data 0-7) = 0-255 (8 bits)
encode:		byte 0 = ((id) << 4) | x = 80 + x
			byte 1 = a
	
			
message id:	(6) led_row2
bytes:		3
format:		iiiiyyyy aaaaaaaa bbbbbbbb
			i (message id) = 6
			y (row to update) = 0-15 (4 bits)
			a (row data 0-7) = 0-255 (8 bits)
			b (row data 8-15) = 0-255 (8 bits)
encode:		byte 0 = ((id) << 4) | y = 96 + y
			byte 1 = a
			byte 2 = b


message id:	(7) led_col2
bytes:		3
format:		iiiixxxx aaaaaaaa bbbbbbbb
			i (message id) = 7
			x (col to update) = 0-15 (4 bits)
			a (col data 0-7) = 0-255 (8 bits)
			b (col data 8-15) = 0-255 (8 bits)
encode:		byte 0 = ((id) << 4) | x = 112 + x
			byte 1 = a
			byte 2 = b


message id:	(8) led_frame
bytes:		9
format:		iiii..qq aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee ffffffff gggggggg hhhhhhhh
			i (message id) = 8
			q (quadrant) = 0-3 (2 bits)
			a-h (row data 0-7, per row) = 0-255 (8 bits)
encode:		byte 0 = ((id) << 4) | q = 128 + q
			byte 1,2,3,4,5,6,7,8 = a,b,c,d,e,f,g,h
note:		quadrants are from top left to bottom right, as shown:
			0 1
			2 3

message id:	(9) clear
bytes:		1
format:		iiii---c
			i (message id) = 9
			c (clear state) = 0-1 (1 bit)
encode:		byte 0 = ((id) << 4) | c = 144 + c
note:		clear state of 0 turns off all leds.
			clear state of 1 turns on all leds.
						

message id:	(10) intensity
bytes:		1
format:		iiiibbbb
			i (message id) = 10
			b (brightness) = 0-15 (4 bits)
encode:		byte 0 = ((id) << 4) | b = 160 + b


message id:	(11) mode
bytes:		1
format:		iiii..mm
			i (message id) = 11
			m (mode) = 0-3 (2 bits)
encode:		byte 0 = ((id) << 4) | m = 176 + m
note:		mode = 0 : normal
			mode = 1 : test (all leds on)
			mode = 2 : shutdown (all leds off)
			


			
			
auxiliary ports

to device:

message id:	(12) activate port
bytes:		1
format:		iiiiaaaa
			i (message id) = 12
			a (which port) = 0-15 (four bits)
encode:		byte 0 = (id) << 4 = 192 + a
			
			
message id:	(13) deactivate port
bytes:		1
format:		iiiiaaaa
			i (message id) = 13
			a (which port) = 0-15 (four bits)
encode:		byte 0 = (id) << 4 = 208 + a
			
			
from device:

message id:	(14) auxiliary input
bytes:		2
format:		iiiiaaaa dddddddd
			i (message id) = 14
			a (port number) = 0-15 (four bits)
			d (data) = 0-255 (eight bits)
decode:		id match: byte 0 >> 4 == 1
			a: byte 0 & 0x0f
			d: byte 1

 */
