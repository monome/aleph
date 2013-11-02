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

// aleph-monome
#include "adc.h"
#include "control.h"
#include "events.h"
#include "midi.h"
#include "monome.h"
#include "timers.h"

// bees
#include "app_timers.h"
#include "render.h"

// tmp
static event_t e;

//------ timers
// refresh the screen periodically
static swTimer_t screenTimer;

// poll monome device 
static swTimer_t monomePollTimer;
// refresh monome device 
static swTimer_t monomeRefreshTimer;


// poll midi device 
static swTimer_t midiPollTimer;
// refresh midi device 
/// TODO:
// static swTimer_t midiRefreshTimer;

// poll adcs 
static swTimer_t adcPollTimer;

// poll midi device
static swTimer_t midiPollTimer;


//----- callbacks

// screen refresh callback
static void screen_timer_callback(int tag) {  
  render_update();
}

//adc polling callback
static void adc_poll_timer_callback(int tag) {
  adc_poll();
}

//midi polling callback
static void midi_poll_timer_callback(int tag) {
  // asynchronous, non-blocking read
  // UHC callback spawns appropriate events
  midi_read();
}

// monome polling callback
static void monome_poll_timer_callback(int tag) {
  // asynchronous, non-blocking read
  // UHC callback spawns appropriate events
  ftdi_read();
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
}

// monome: start polling
void timers_set_monome(void) {
  print_dbg("\r\n setting monome timers");
  set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1);
  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 50,  &monome_refresh_timer_callback, 1);
}

// monome stop polling
void timers_unset_monome(void) {
  print_dbg("\r\n unsetting monome timers");
  kill_timer(eMonomePollTimerTag);
  kill_timer(eMonomeRefreshTimerTag); 
}

// midi : start polling
void timers_set_midi(void) {
  print_dbg("\r\n setting midi timers");
  set_timer(&midiPollTimer,    eMidiPollTimerTag,    20,  &midi_poll_timer_callback,    1);
  // TODO:
  //  set_timer(&midiRefreshTimer, eMidiRefreshTimerTag, 50,  &midi_refresh_timer_callback, 1);
}

// midi : stop polling
void timers_unset_midi(void) {
  print_dbg("\r\n unsetting midi timers");
  kill_timer(eMidiPollTimerTag);
  // TODO:
  //  kill_timer(eMidiRefreshTimerTag); 
}

// adc : start polling
void timers_set_adc(void) {
  print_dbg("\r\n setting adc timers");
  set_timer(&adcPollTimer,    eAdcPollTimerTag,    20,  &adc_poll_timer_callback,    1);
}

// adc : stop polling
void timers_unset_adc(void) {
  print_dbg("\r\n unsetting adc timers");
  kill_timer(eAdcPollTimerTag);
} 
