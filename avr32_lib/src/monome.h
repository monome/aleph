#ifndef _MONOME_H_
#define _MONOME_H_

#include "events.h"
#include "ftdi.h"
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

// how many leds in 1 row of the buffer
#define MONOME_LED_ROW_BYTES 16
// define mul as lshift
#define MONOME_LED_ROW_LS 4

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

// device enumeration
typedef enum {
  eDeviceGrid,   /// any grid device
  eDeviceArc,     /// any arc device
  eDeviceNumDevices // dummy and count
} eMonomeDevice;

//--------------------------------
//------- variables

// connected flag ( TODO: device id? )
//== use events instead
// extern u8 monomeConnect;

// dirt flags for each frame, as bitfield
extern u8 monomeFrameDirty;

// a buffer big enough to hold all led data for 256 or arc4
// each led gets a full byte
extern u8 monomeLedBuffer[MONOME_MAX_LED_BYTES];

//---- function types
/*
  define protocol-agnostic function types
  function pointers of the appropriate types will be maintained
  and assigned according to protocol selection.
 */

////// read raw serial data (all devices)
typedef void(*read_serial_t)(void);
//// set intensity
// set led intensity of connected device
typedef void(*set_intense_t)(u8 level);

///// write (grid)extern 
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

//// refresh (grid vs arc)
typedef void(*refresh_t)(void);

// global pointers to function types defined above.
// assigned according to detected device protocol.
/// FIXME: will need multiple device support eventually.
extern read_serial_t monome_read_serial;
extern set_intense_t monome_set_intense;
extern grid_led_t monome_grid_led;
extern grid_map_t monome_grid_map;
extern grid_level_map_t monome_grid_level_map;
extern ring_map_t monome_ring_map;
extern refresh_t monome_refresh;

//-------------------------------------
//------ functions

// initialize
extern void init_monome(void);
// check monome device  from FTDI string descriptors
extern u8 check_monome_device_desc(char* mstr, char* pstr, char* sstr);

// check dirty flags and refresh leds
extern void monome_grid_refresh(void);

// check dirty flags and refresh leds
extern void monome_arc_refresh(void);

/*
  monome_*_parse_event_data :
  convert event data as handled by the event queue,
  into useful parameters, depending on the type of event.
 */

// connection event
// parameters: device id, size a. size b
// (a, b) = (width, height) for grids
// (a, b) = (rings, leds/ring) for arcs
extern void monome_connect_parse_event_data(u32 data, eMonomeDevice * dev, u8* w, u8* h);

// disconnect
// (no data needed, so no parse)
//extern void monome_disconnect_parse_event_data(u32 data, u8* x, u8* y, u8* val);

// grid presses
// parameters: column, row, on/off value
extern void monome_grid_key_parse_event_data(u32 data, u8* x, u8* y, u8* val);

// grid tilt / adc
// parameters: which axis, 16-bit value
extern void monome_grid_adc_parse_event_data(u32 data, u8* n, u16* val);

// ring encoder
// parameters: which ring, delta value
extern void monome_ring_enc_parse_event_data(u32 data, u8* n, s8* delta);

// ring press/lift
// parameters: which ring, on/off value
extern void monome_ring_key_parse_event_data(u32 data, u8* n, u8* val);

/*
  led_set, led_toggle
  these are top-level functions to set or toggle a single led.
  apps can use these, or directly manipulate led/dirty buffers.
  there's no great speed difference so use whichever suits the situation.
*/
// grid led/set function
extern void monome_grid_led_set(u8 x, u8 y, u8 val);

// grid led/toggle function
extern void monome_grid_led_toggle(u8 x, u8 y);

// arc led/set function
extern void monome_arc_led_set(u8 enc, u8 ring, u8 val);


/// TODO: add these row functions,
/// and abstract some of the stuff in e.g. dsyn/grid.c
//  column-set, etc probably less useful due to our data layout.
//
// top-level row/set function
// extern void monome_row_set(u8 y, u8* val);
// top-level row/toggle function
// extern void monome_row_toggle(u8 y, u8* mask);


/*
  dirty-flag maintenance functions.
  a bitfield is used to signal updates to each of the 8x8 frames in a monome.
  apps can set the bitfield 
  FIXME: really, better off without this.
  add row/col/frame functions that deal with dirty flags internally.
 */

// set quadrant dirty flag from (x,y)
extern void monome_calc_quadrant_flag(u8 x, u8 y);
// set given quadrant dirty flag
extern void monome_set_quadrant_flag(u8 q);

// convert flat framebuffer idx to x,y
extern void monome_idx_xy(u32 idx, u8* x, u8* y);
// convert x,y to framebuffer idx
extern u32 monome_xy_idx(u8 x, u8 y);

extern u8 monome_size_x(void);
extern u8 monome_size_y(void);
extern u8 monome_is_vari(void);

extern eMonomeDevice monome_dev_type(void);


#endif // h guard
