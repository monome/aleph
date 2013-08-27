/* app.h
   avr32
   aleph

   this is the public API for aleph avr32 controller applications.
   
*/

#ifndef _ALEPH_AVR32_APP_H_
#define _ALEPH_AVR32_APP_H_

#include "events.h"
#include "types.h"
//--------------------------------
//---- event handler function types
//--- handler functions specialized for each type of event.

// event handler function pointer type
typedef void (*event_handler)(event_t* ev);
// application's current event handler
extern event_handler appEventHandler;

// switch, emits state
/* typedef void (*sw_handler)(u8 state); */
/* // encoder emits signed value at the time it was polled. */
/* // note that the range of this value is a function of the pollling period. */
/* typedef void (*enc_handler)(s32 val); */
/* // adc emits unsigned 12-bit value. */
/* typedef void (*adc_handler)(u16 val); */
/* // monome key press */
/* typedef void (*monome_key_handler)(u16 x, u16 y, u8 val); */
// monome ring turn
//typedef void (*monome_ring_handler)(void/* TODO */);

// midi note
// midi cc
// HID mouse
// HID key
// HID generic
///... ?

///----- handler pointers
//-- these should be initialized to NULL in the core, and assigned by the app.
//-- core should obviously never attempt to use a null-valued handler.

// handlers for function switches

/* sw_handler fnSwHandler[4]; // = { NULL, NULL, NULL, NULL }; */
/* // handler for mode switch */
/* sw_handler modeSwHandler; */
/* // handlers for footswitches */
/* sw_handler footSwHandler[2]; */
/* // handlers for the 4 encoders */
/* enc_handler encHandler[4]; */
/* // handlers for adcs */
/* adc_handler adcHandler[4]; */
/* // monome grid press handler */
/* monome_key_handler gridHandle; */
// mouse handler
// keyboard handler
// generic HID handler

//-------------------------------------
//-- these should be implemented by the app,
//-- e.g. aleph/avr32/apps/foo/src/app_foo.c

// called at startup to allocate memory. 
extern void app_init(void);

// called from avr32 event loop when startup is finished and UI is running.
// argument is a flag if this is the device's first run since chiperase.
// should assign event handlers here
// returns success/failure
extern u8 app_launch(u8 firstrun);

//-------------------------------------- -
//--- these are implmeneted by avr32 core 

// stop performing UI events
extern void app_pause(void);
// stop and display start/stop messages
extern void app_notify(const char* msg);
// resume UI
extern void app_resume(void);

#endif // h guard
