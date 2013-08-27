/*
  monome.h
  aleph-avr32
  
  monome device interface declarations

 */

#ifndef _ALEPH_MONOME_INTERFACE_H_
#define _ALEPH_MONOME_INTERFACE_H_

#include "events.h"
#include "types.h"

//--------------------------------
//------ defines


////// data sizes
/// maximum unpacked size of led buffer,
/// all frames/elements, 1 byte per led.
// arc4 and 256 are tied.
#define MONOME_MAX_LED_BYTES 256
// how many leds on each side of a quad
#define MONOME_QUAD_LEDS 8
//-- sizes for the actual data to be send over usb
// one frame is an 8x8 quadrant; 256 has 4.
#define MONOME_GRID_MAX_FRAMES 4
// map stores binary data for each led in 8x8
#define MONOME_GRID_MAP_BYTES 8
// level map stores 8b level data for each led in 8x8
/// TODO:
//#define MONOME_GRID_LEVEL_MAP_SIZE 64

// a frame is one knob, max is arc4
#define MONOME_RING_MAX_FRAMES 4
// map is varibright, 4 bytes per led, 64 leds
#define MONOME_RING_MAP_SIZE  32


//---------------------------------
//--- types

////// read (all devices)
typedef void(*read_serial_t)(void);

///// write (grid)
// single led
typedef void(*grid_led_t)(u8 x, u8 y, u8 val);
// binary 8x8 frame
typedef void(*grid_map_t)(u8 x, u8 y, const u8* data);
// 8x8x8b varibright frame
typedef void(*grid_level_map_t)(u8 x, u8 y, const u8* data);
///// write (ring)
// set single led in ring with 4b value
typedef void(*ring_set_t)(u8 n, u8 rho, u8 val);
// set all leds in ring 
typedef void(*ring_map_t)(u8 n, u8* data);

//--------------------------------
//------- variables
// connected flag ( TODO: device id? )
extern u8 monomeConnect;
// dirt flags for each frame, as bitfield
extern u8 monomeFrameDirty;
// a buffer big enough to hold all led data for 256 or arc4
// each led gets a full byte
extern u8 monomeLedBuffer[MONOME_MAX_LED_BYTES];

// global pointers to function types defined above.
// assigned according to detected device protocol.
extern read_serial_t monome_read_serial;
extern grid_led_t monome_grid_led;
extern grid_map_t monome_grid_map;
extern grid_level_map_t monome_grid_level_map;
extern ring_map_t monome_ring_map;

//------ functions

// initialize
extern void init_monome(void);
// check monome device  from FTDI string descriptors
extern u8 check_monome_device_desc(char* mstr, char* pstr, char* sstr);
// check dirty flags and refresh leds
extern void monome_grid_refresh(void);

// convert event data

// grid presses
extern void monome_grid_key_parse_event_data(u32 data, u8* x, u8* y, u8* val);
// grid tilt / adc
extern void monome_grid_adc_parse_event_data(u32 data, u8* n, u16* val);
// ring encoder
extern void monome_ring_enc_parse_event_data(u32 data, u8* n, s8* delta);
// ring press/lift
extern void monome_ring_key_parse_event_data(u32 data, u8* n, u8* val);

// set quadrant dirty flag
extern void monome_calc_quadrant_flag(u8 x, u8 y);
// convert flat framebuffer idx to x,y
extern void monome_idx_xy(u32 idx, u8* x, u8* y);
// convert x,y to framebuffer idx
extern u32 monome_xy_idx(u8 x, u8 y);

#endif // h guard
