/* app_timers.c
   
   setup up application software timers.
   TODO: should really split some of these off from avr32 core to app
 */


// ASF
#include "compiler.h"
#include "delay.h"
#include "gpio.h"
#include "math.h"
#include "print_funcs.h"
#include "uhc.h"
// aleph-avr32
#include "adc.h"
#include "control.h"
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "ftdi.h"
#include "midi.h"
#include "monome.h"
#include "global.h"
#include "switches.h"
#include "timers.h"
#include "types.h"
#include "uhi_ftdi.h"

#include "sys_timers.h"


//----- static variables

//--- timers
// refresh the screen periodically
// static swTimer_t screenTimer;
// poll encoder accumulators periodically
static swTimer_t encTimer;

// poll switches
//static swTimer_t swTimer;
// poll ADC
static swTimer_t adcTimer;
// poll monome device
static swTimer_t monomePollTimer;
// refresh monome device
static swTimer_t monomeRefreshTimer;
/* // poll midi device */
/* static swTimer_t midiPollTimer; */
// refresh midi device
//static swTimer_t midiRefreshTimer;
//// FIXME: this is bees-specific so should go in app code.
//// will make a general overhall of this timers module.
// send param changes
static swTimer_t paramChangeTimer;

//--- static misc
static u8 i;
static event_t e;
			       
//----- callbacks

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
      //      print_dbg("\r\n posting encoder event, num: ");'
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

// midi polling callback
static void param_change_timer_callback(int tag) {
  ctl_perform_all_changes();
}



// monome polling callback
static void monome_poll_timer_callback(int tag) {
  if (monomeConnect > 0) {
    // start an ftdi transfer, callback handles event posting
    ftdi_read();
  }
}

//monome refresh callback
static void monome_refresh_timer_callback(int tag) {
  if (monomeConnect) {
    //    print_dbg("\r\n posting monome refresh event");
    if(monomeFrameDirty > 0) {
      e.eventType = kEventMonomeRefresh;
      post_event(&e);
    }
  }
}


//====== external
void init_sys_timers(void) {
  set_timer(&encTimer,           eEncTimerTag,           20,  &enc_timer_callback,    1);
  set_timer(&adcTimer,           eAdcTimerTag,           20,  &adc_timer_callback,    1);
  set_timer(&monomePollTimer,    eMonomePollTimerTag,      20,  &monome_poll_timer_callback, 1);
  //  set_timer(&monomePollTimer,    eMonomePollTimerTag,      5,  &monome_poll_timer_callback, 1);
  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 20,  &monome_refresh_timer_callback, 1);
  set_timer(&paramChangeTimer,   eParamChangeTimerTag,      1,  &param_change_timer_callback, 1);
}
