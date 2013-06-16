// ASF
#include "compiler.h"
#include "gpio.h"
#include "math.h"
#include "print_funcs.h"
// aleph
#include "adc.h"
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
// poll switches
//static swTimer_t swTimer;
// poll ADC
static swTimer_t adcTimer;

//--- static misc
static u8 i;
static event_t e;

//----- callbacks
// screen refresh callback
static void screen_timer_callback(int tag) {  
  
  //// PROFILE
  /*
  static U64 cycles = 0;
  static U64 cyclesNow = 0;
  
  cyclesNow = Get_system_register(AVR32_COUNT);
  print_dbg(" \lcycles:"); print_dbg_ulong(cyclesNow - cycles);
  cycles = cyclesNow;
  */

  if(refresh) {
    e.eventType = kEventRefresh;
    post_event(&e);
    refresh = 0;
  }
}

// encoder accumulator polling callback
//// FIXME optimize
static void enc_timer_callback(int tag) {
  static s16 val;
  for(i=0; i<NUM_ENC; i++) {
    val = enc[i].val;
    if ( (val > enc[i].thresh) || (val < (enc[i].thresh * -1)) ) {
    //  if( (val > ENC_THRESH) || (val < ENC_NEG_THRESH) ) {
      e.eventType = enc[i].event;
      e.eventData = val;
      enc[i].val = 0;
      post_event(&e);
      //      print_dbg("\r\n posting encoder event, num: ");
      //      print_dbg_ulong(i);
      //      print_dbg(" , val: ");
      //      print_dbg_ulong(val);
    }
  }
}

// adc polling callback
static void adc_timer_callback(int tag) {
  adc_poll();
}

//====== external
void init_app_timers(void) {
  set_timer(&screenTimer, eScreenTimerTag, 30,   &screen_timer_callback, 1);
  set_timer(&encTimer,    eEncTimerTag,    20,    &enc_timer_callback,    1);
  set_timer(&adcTimer,    eAdcTimerTag,    5,    &adc_timer_callback,    1);
}
