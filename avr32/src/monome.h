/*
  monome.h
  aleph-avr32
  
  monome device interface declarations

 */

#ifndef _ALEPH_MONOME_INTERFACE_H_
#define _ALEPH_MONOME_INTERFACE_H_

#include "types.h"

//----- types
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
  u8 knobs; // number of encoders
  u8 tilt;  // has tilt (??)  
} monomeDesc;

//------- variables
extern u8 monomeConnect;

//------ functions

// check monome device  from FTDI string descriptors
extern u8 check_monome_device(char* mstr, char* pstr, char* sstr);

// read serial
extern void monome_read_serial(void);

// transmit various messages
extern void grid_led(u8 x, u8 y, u8 val);
extern void grid_col(u8 x, u8 val);
extern void grid_row(u8 y, u8 val);
extern void arc_led(u8 n, u8 idx, u8 val);

#endif // h guard
