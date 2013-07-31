/*
  monome.h
  aleph-avr32
  
  monome device interface declarations

 */

#ifndef _ALEPH_MONOME_INTERFACE_H_
#define _ALEPH_MONOME_INTERFACE_H_

#include "events.h"
#include "types.h"

//----- types
//eh...
/* typedef struct _monomeGridEvent { */
/* } monomeGridEvent_t; */

//------- variables
extern u8 monomeConnect;

//------ functions

// check monome device  from FTDI string descriptors
extern u8 check_monome_device(char* mstr, char* pstr, char* sstr);

// read serial
extern void monome_read_serial(void);

// transmit various messages
extern void monome_grid_led(u8 x, u8 y, u8 val);
extern void monome_grid_col(u8 x, u8 val);
extern void monome_grid_row(u8 y, u8 val);
extern void monome_arc_led(u8 n, u8 idx, u8 val);

// convert event data
extern void monome_grid_write_event(event_t* ev, u8 x, u8 y, u8 val);
extern void monome_grid_read_event(event_t* ev, u8* x, u8* y, u8* val);
extern void monome_tilt_write_event(event_t* ev, u8 axis, u16 val);
extern void monome_tilt_read_event(event_t* ev, u8* axis, u16* val););

#endif // h guard
