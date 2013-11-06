// ASF
#include "compiler.h"
#include "gpio.h"
#include "math.h"
// aleph
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "global.h"
#include "switches.h"
#include "timers.h"
#include "types.h"
#include "app_timers.h"

//----- timers
// refresh the screen periodically
static swTimer_t screenTimer;
// poll encoder accumulators periodically
static swTimer_t encTimer;

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
  static s16 val;
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
//====== external
void init_app_timers(void) {
  set_timer(&screenTimer, eScreenTimerTag, 30,   &screen_timer_callback, 1);
  set_timer(&encTimer,    eEncTimerTag,    20,    &enc_timer_callback,    1);
}
