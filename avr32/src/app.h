/* app.h
   avr32
   aleph

   this is the public API for aleph avr32 controller applications.
*/

#ifndef _ALEPH_AVR32_APP_H_
#define _ALEPH_AVR32_APP_H_

#include "events.h"
#include "types.h"

//-------------------------------------
//-- these should be implemented by the app,
//-- e.g. aleph/avr32/apps/foo/src/app.c

// called at startup to allocate memory. 
extern void app_init(void);
// called from avr32 event loop when startup is finished and UI is running
// argument is a flag if this is the device's first run since chiperase
extern void app_launch(u8 firstrun);
// handle UI events
extern void app_handle_event(event_t* e);

//-------------------------------------- -
//--- these are implmeneted by avr32 core 

// stop performing UI events
extern void app_pause(void);
// stop and display start/stop messages
extern void app_notify(const char* startMsg, const char* stopMsg);
// resume UI
extern void app_resume(void);

#endif // h guard
