/* app_timers.c

   this is where callbacks are declared for all application-specific software timers.
   these callbacks are performed from the TC interrupt service routine.
   therefore, they should be kept small.

   a good strategy is to create application event types in app_events.h
   and have the timer callbacks post these events to the queue.
   the main loop will service the queue and call the application event handler.
 */

//asf
#include "print_funcs.h"

/// avr32_lib
#include "encoders.h"
#include "events.h"
#include "monome.h"
#include "timers.h"
//#include "midi.h"
#include "app_timers.h"
#include "ctl.h"
#include "control.h"
#include "render.h"

// tmp
static event_t e;

//------ timers
// refresh the screen periodically
static softTimer_t screenTimer = { .next = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL };

// poll monome device 
static softTimer_t monomePollTimer = { .next = NULL };
// refresh monome device 
static softTimer_t monomeRefreshTimer = { .next = NULL };

//----- callbacks

// screen refresh callback
static void screen_timer_callback(void* obj) {  
  render_update();
}

// encoder accumulator polling callback
static void enc_timer_callback(void* obj) {
  static s16 val, valAbs;
  u8 i;

  for(i=0; i<NUM_ENC; i++) {
    val = enc[i].val;
    valAbs = (val & 0x8000 ? (val ^ 0xffff) + 1 : val);
    if(valAbs > enc[i].thresh) {
      e.type = enc[i].event;
      e.data = val;
      enc[i].val = 0;
      event_post(&e);
    }
  }
}

// monome polling callback
static void monome_poll_timer_callback(void* obj) {
  ftdi_read();
}

// monome refresh callback
static void monome_refresh_timer_callback(void* obj) {
  if(monomeFrameDirty > 0) {
    e.type = kEventMonomeRefresh;
    event_post(&e);
  }
}

//----------------------------
//---- external functions

void init_app_timers(void) {
  timer_add(&screenTimer, 50,  &screen_timer_callback,  NULL);
  timer_add(&encTimer, 50, &enc_timer_callback, NULL );
}

 void timers_set_monome(void) {
   print_dbg("\r\n setting monome timers");
  timer_add(&monomePollTimer, 20,  &monome_poll_timer_callback,    NULL);
  timer_add(&monomeRefreshTimer, 50,  &monome_refresh_timer_callback, NULL);
}

 void timers_unset_monome(void) {
   print_dbg("\r\n unsetting monome timers");
  timer_remove(&monomePollTimer);
  timer_remove(&monomeRefreshTimer); 
}
