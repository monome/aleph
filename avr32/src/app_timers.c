// ASF
#include "compiler.h"
#include "math.h"
#include "print_funcs.h"
// aleph
#include "adc.h"
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "global.h"
#include "timers.h"
#include "types.h"
#include "app_timers.h"


//----- timers
// refresh the screen periodically
static timer_t screenTimer;
// poll encoder accumulators periodically
static timer_t encTimer;
// debounce switches
//static timer_t swTimer;
// poll ADC
static timer_t adcTimer;

//--- static misc
static u8 i;
static event_t e;

//----- callbacks
// screen refresh callback
static void screen_timer_callback(int tag) {  
  if(refresh) {
    e.eventType = kEventRefresh;
    post_event(&e);
    refresh = 0;
  }
}

// encoder accumulator polling callback
static void enc_timer_callback(int tag) {
  s16 val;
  for(i=0; i<NUM_ENC; i++) {
    val = enc[i].val;
    if ( (val > enc[i].thresh) || (val < (enc[i].thresh * -1)) ) {
      e.eventType = enc[i].event;
      e.eventData = val;
      enc[i].val = 0;
      post_event(&e);
    }
  }
}

/*
// switch debouncing callback
static void sw_timer_callback(int tag) {
  // TODO
}
*/

// adc polling callback
static void adc_timer_callback(int tag) {
  // print_dbg("\r\n polling adc from timer callback...");
  adc_poll();
}

//====== external
void init_app_timers(void) {
  set_timer(&screenTimer, eScreenTimerTag, 5,   &screen_timer_callback, 1);
  set_timer(&encTimer,    eEncTimerTag,    2,   &enc_timer_callback,    1);
  set_timer(&adcTimer,    eAdcTimerTag,    1000, &adc_timer_callback,    1);
}
