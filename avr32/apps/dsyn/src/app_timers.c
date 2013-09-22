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

#include "events.h"
#include "monome.h"
#include "timers.h"
//#include "midi.h"
#include "app_event_types.h"
#include "app_timers.h"
#include "ctl.h"
#include "control.h"
#include "render.h"

// tmp
static event_t e;

//------ timers
// refresh the screen periodically
static swTimer_t screenTimer;
// poll adcs 
//static swTimer_t adcPollTimer;

// poll monome device 
static swTimer_t monomePollTimer;
// refresh monome device 
static swTimer_t monomeRefreshTimer;
// poll midi device
//static swTimer_t midiPollTimer;

static swTimer_t metroTimer;
//static swTimer_t sustainTimer;

//----- callbacks
//static void sustain_timer_callback(int tag);


// screen refresh callback
static void screen_timer_callback(int tag) {  
  render_update();
}

// metronome timer callback
static void metro_timer_callback(int tag) {
  //static u32 i = 0;
  e.eventType = kEventSeqNext;
  post_event(&e);
  /*
    print_dbg(" > ");
    // : a test pattern.
    if( ((i % 5)==0) || ((i % 2)==0) ) {
      // gate on. should be using triggered mode.
      ctl_voice_param(0, eParamGate, 1);
    }
    if( ((i % 7)==0) || ((i % 3)==0) ) {
      ctl_voice_param(1, eParamGate, 1);
    }
    i++;
  */
}


/// might want these... 
/* //1-shot sustain callback */
/* void sustain_timer_callback(int tag) { */
/*   // turn gate off */
/*   ctl_param_change(eParamGate, 0); */
/* } */

/* //adc polling callback */
/* static void adc_timer_callback(int tag) { */
/*   adc_poll(); */
/* } */

/* //midi polling callback */
/* static void midi_poll_timer_callback(int tag) { */
/*   if(midiConnect) { */
/*     midi_read(); */
/*   } */
/* } */



/// FIXME: should do small optimizations.
// for example, can set/unset the sw timer 
// instead of always processing + conditional
// monome polling callback
static void monome_poll_timer_callback(int tag) {
  //  if (monomeConnect > 0) {
    // start an ftdi transfer, callback handles event posting
    ftdi_read();
    //  }
}

// monome refresh callback
static void monome_refresh_timer_callback(int tag) {
  //  if (monomeConnect) {
    //    print_dbg("\r\n posting monome refresh event");
    if(monomeFrameDirty > 0) {
      e.eventType = kEventMonomeRefresh;
      post_event(&e);
    }
    //  }
}

//----------------------------
//---- external functions

void init_app_timers(void) {
  set_timer(&screenTimer,        eScreenTimerTag,        50,  &screen_timer_callback,  1);
  //  set_timer(&adcTimer,           eAdcTimerTag,           5,   &adc_timer_callback,     1);
  //  set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1);
  //  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 20,  &monome_refresh_timer_callback, 1);
  //  set_timer(&midiPollTimer,      eMidiPollTimerTag,      5,  &midi_poll_timer_callback, 1);
  set_timer(&metroTimer,        eMetroTimerTag,        1000,  &metro_timer_callback,  1);
}

// set the metro timer period
 void timers_set_metro_ms(u32 ms) {
  // doing this will effectively pause the timer while knob moves
  //  metroTimer.timeout = ms;
  metroTimer.timeoutReload = ms;
}

 void timers_set_monome(void) {
   print_dbg("\r\n setting monome timers");
  set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1);
  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 50,  &monome_refresh_timer_callback, 1);
}

 void timers_unset_monome(void) {
   print_dbg("\r\n unsetting monome timers");
  kill_timer(eMonomePollTimerTag);
  kill_timer(eMonomeRefreshTimerTag); 
}
