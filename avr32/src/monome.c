/*
  monome.c
  aleph-avr32

  monome device interface definitions
 */

// asf
#include "print_funcs.h"
#include <string.h>
// aleph
#include "ftdi.h"
#include "monome.h"


//------ defines

// manufacturer string length
#define MONOME_MANSTR_LEN 6
// product string length
#define MONOME_PRODSTR_LEN 8
// serial string length
#define MONOME_SERSTR_LEN 9

//---- typedefs
// function pointer types

typedef void(*read_serial_t)(void);
typedef void(*grid_led_t)(u8 x, u8 y, u8 val);
typedef void(*grid_col_t)(u8 x, u8 val);
typedef void(*grid_row_t)(u8 y, u8 val);
typedef void(*arc_led_t)(u8 n, u8 idx, u8 val);

//------ extern variables
// connected flag
u8 monomeConnect = 0;

//----- static variables
// descriptor for connected device
static monomeDesc mdesc = {
  .protocol = eProtocolNumProtocols, // dummy
  .device = eDeviceNumDevices, // dummy
  .cols = 0,
  .rows = 0,
  .knobs = 0,
  .tilt = 0,
};

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

static void grid_col_40h(u8 x, u8 val);
static void grid_col_series(u8 x, u8 val);
static void grid_col_mext(u8 x, u8 val);

static void grid_row_40h(u8 y, u8 val);
static void grid_row_series(u8 y, u8 val);
static void grid_row_mext(u8 y, u8 val);

static void arc_led_mext(u8 n, u8 idx, u8 val);

// current function pointers
static read_serial_t readSerialFunc;
static grid_led_t gridLedFunc;
static grid_col_t gridColFunc;
static grid_row_t gridRowFunc;
static arc_led_t arcLedFunc;

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

static grid_col_t gridColFuncs[eProtocolNumProtocols] = {
  &grid_col_40h,
  &grid_col_series,
  &grid_col_mext,
};

static grid_row_t gridRowFuncs[eProtocolNumProtocols] = {
  &grid_row_40h,
  &grid_row_series,
  &grid_row_mext,
};

static arc_led_t arcLedFuncs[eProtocolNumProtocols] = {
  NULL, // unsupported
  NULL, // unsupported
  &arc_led_mext,
};



//================================================
//----- extern function definitions

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
    // we need to query for device type, size, and other properties.
    mdesc.protocol = eProtocolSeries;
    return setup_mext();
  }
}

extern void monome_read_serial(void) {
  (*readSerialFunc)();
}

extern void grid_led(u8 x, u8 y, u8 val) {
  (*gridLedFunc)(x, y, val);
}

extern void grid_col(u8 x, u8 val) {
  (*gridColFunc)(x, val);
}

extern void grid_row(u8 y, u8 val) {
  (*gridRowFunc)(y, val);
}

extern void arc_led(u8 n, u8 idx, u8 val) {
  (*arcLedFunc)(n, idx, val);
}

//=============================================
//------ static function definitions

// set function pointers
static inline void set_funcs(void) {
  readSerialFunc = readSerialFuncs[mdesc.protocol];
  gridLedFunc = gridLedFuncs[mdesc.protocol];
  gridColFunc = gridColFuncs[mdesc.protocol];
  gridRowFunc = gridRowFuncs[mdesc.protocol];
  arcLedFunc = arcLedFuncs[mdesc.protocol];
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
  ///// TODO.. 
  set_funcs();

  if(/*success*/ 0 ) {
    monomeConnect = 1;
    return 1;
  } else {
    return 0;
  }
}

//----- protocol-specific functions

//--- rx
// rx for each protocol
static void read_serial_40h(void) {
}

static void read_serial_series(void) {
  u8 i;
  u8 nb = ftdi_read();
  //  print_dbg("\r\n monome read_serial_series");
  //  print_dbg("\r\n bytes read: ");
  // print_dbg_ulong(nb);
  //  print_byte_array(ftdiRxBuf, nb, 0);
  if(nb > 2) {
    print_dbg("\r\n monome in: ");
    for(i=2; i<nb; i++) {
      print_dbg_hex(ftdiRxBuf[i]);
      print_dbg(" ");
    }
    print_dbg("\r\n");
  }
}

static void read_serial_mext(void) {
}

//--- tx
static void grid_led_40h(u8 x, u8 y, u8 val) {
}

static void grid_led_series(u8 x, u8 y, u8 val) {
}

static void grid_led_mext(u8 x, u8 y, u8 val) {
}

static void grid_col_40h(u8 x, u8 val) {
}

static void grid_col_series(u8 x, u8 val) {
}

static void grid_col_mext(u8 x, u8 val) {
}

static void grid_row_40h(u8 y, u8 val) {
}

static void grid_row_series(u8 y, u8 val) {
}

static void grid_row_mext(u8 y, u8 val) {
}

static void arc_led_mext(u8 n, u8 idx, u8 val) {
}


