/* app.h
   avr32
   aleph

   this is the public API for aleph avr32 controller applications.
   
*/

#ifndef _ALEPH_AVR32_APP_H_
#define _ALEPH_AVR32_APP_H_

#include "event_types.h"
#include "events.h"
#include "types.h"

// event handler function type
//typedef void (*app_event_handler)(s32 data);

// global array of event handlers
//extern app_event_handler ** appEventHandlers;

// global pointer to array of pointers to handlers
//extern void (*(*app_event_handlers)[])(s32 data);

// global array of pointers to handlers
extern void (*app_event_handlers[])(s32 data);

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
