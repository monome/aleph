//asf
#include "print_funcs.h"

#include "events.h"
#include "monome.h"
#include "timers.h"
//#include "midi.h"

#include "app_timers.h"
#include "ctl.h"
#include "control.h"
#include "render.h"

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
  static u32 i = 0;
    print_dbg(" > ");
    //// 
    // fixme: a test pattern.
    // sequencer logic stuff shouldn't happen in an isr. 
    // application needs its own event queue or some way of defining new main events.

    if( ((i % 5)==0) || ((i % 2)==0) ) {
      // gate on. should be using triggered mode.
      ctl_voice_param(0, eParamGate, 1);
    }
    if( ((i % 7)==0) || ((i % 3)==0) ) {
      ctl_voice_param(1, eParamGate, 1);
    }
    i++;
}


// 1-shot sustain callback
/* void sustain_timer_callback(int tag) { */
/*   // turn gate off   */
/*   ctl_param_change(eParamGate, 0); */
/* } */

/////////////////////////////
//////////////
//////  not using these:


// adc polling callback
/* static void adc_timer_callback(int tag) { */
/*   adc_poll(); */
/* } */

// monome polling callback
static void monome_poll_timer_callback(int tag) {
  if (monomeConnect > 0) {
    // start an ftdi transfer, callback handles event posting
    ftdi_read();
  }
}

// monome refresh callback
static void monome_refresh_timer_callback(int tag) {
  static event_t e;
  if (monomeConnect) {
    //    print_dbg("\r\n posting monome refresh event");
    if(monomeFrameDirty > 0) {
      e.eventType = kEventMonomeRefresh;
      post_event(&e);
    }
  }
}

/* //midi polling callback */
/* static void midi_poll_timer_callback(int tag) { */
/*   if(midiConnect) { */
/*     midi_read(); */
/*   } */
/* } */


//====== external
void init_app_timers(void) {
  set_timer(&screenTimer,        eScreenTimerTag,        20,  &screen_timer_callback,  1);
  //  set_timer(&adcTimer,           eAdcTimerTag,           5,   &adc_timer_callback,     1);
  set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1);
  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 20,  &monome_refresh_timer_callback, 1);
  //  set_timer(&midiPollTimer,      eMidiPollTimerTag,      5,  &midi_poll_timer_callback, 1);
  set_timer(&metroTimer,        eMetroTimerTag,        1000,  &metro_timer_callback,  1);
}

// set the metro timer period
extern void timers_set_metro_ms(u32 ms) {
  //  metroTimer.timeout = ms;
  metroTimer.timeoutReload = ms;
}
