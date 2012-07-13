// ASF
#include "compiler.h"
#include "math.h"
// aleph
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
static timer_t swTimer;

//--- static misc
static u8 i;
static event_t e;

//----- callbacks
static void screen_timer_callback(int tag) {  
  if(refresh) {
    e.eventType = kEventRefresh;
    post_event(&e);
    refresh = 0;
  }
}

static void enc_timer_callback(int tag) {
  s16 val;
  for(i=0; i<NUM_ENC; i++) {
    //     if(enc[i].val != 0) {
    val = enc[i].val;
   if ( (val > enc[i].thresh) || (val < (enc[i].thresh * -1)) ) {
       e.eventType = enc[i].event;
       e.eventData = val;
       enc[i].val = 0;
       post_event(&e);
    }
  }
  /*
  u16 val;
  //  s16 lowthresh;
  // check accumulators of all encoders
  for(i=0; i<NUM_ENC; i++) {
    val = enc[i].val;
    // FIXME: slow
    if ( (val > enc[i].thresh) || (val < (enc[i].thresh * -1)) ) {
      e.eventType = enc[i].event;
      if (val > 0) { e.eventData = 1; }
      else if (val < 0) { e.eventData = -1; }
      enc[i].val = 0;
      post_event(&e);
    }
  }
  */
}

static void sw_timer_callback(int tag) {
}

//====== external
void init_app_timers(void) {
  set_timer(&screenTimer, eScreenTimerTag, 5, &screen_timer_callback, 1);
  set_timer(&encTimer, eEncTimerTag, 2, &enc_timer_callback, 1);
}
