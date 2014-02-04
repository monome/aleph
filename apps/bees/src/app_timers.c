/* app_timers.c

   this is where callbacks are declared for all application-specific software timers.
   these callbacks are performed from the TC interrupt service routine.
   therefore, they should be kept small.

   avr32_lib defines a custom application event type, 
   which should be used when timer-based processing needs to be deferred to the main loop.
*/

//asf
#include "print_funcs.h"

// aleph-avr32
#include "adc.h"
#include "control.h"
#include "encoders.h"
#include "events.h"
#include "midi.h"
#include "monome.h"
#include "net_poll.h"
#include "timers.h"

// bees
#include "ops/op_metro.h"
#include "app_timers.h"
#include "render.h"

//---------------------------
//---- static variables

// event 
static event_t e;

//------ timers
// refresh screen
static softTimer_t screenTimer = { .next = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL };

// poll monome device 
static softTimer_t monomePollTimer = { .next = NULL };

// refresh monome device 
static softTimer_t monomeRefreshTimer  = { .next = NULL };

// poll midi device 
static softTimer_t midiPollTimer = { .next = NULL };

// refresh midi device 
/// TODO:
// static softTimer_t midiRefreshTimer;

// poll adc 
static softTimer_t adcPollTimer = { .next = NULL };


//--------------------------
//----- static functions

//----- callbacks

// the system defines a single event type for application events.
// event data is a pointer to an arbitrary object
// here we use it for polled operators like op_metro.
static void app_custom_event_callback(void* obj) {
  // print_dbg("\r\n bees, app_custom_event_callback, obj: 0x");
  // print_dbg_hex((u32)obj);
  e.type = kEventAppCustom;
  // post the object's address in the event data field
  e.data = (s32)obj;
  event_post(&e);
}

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


//adc polling callback
static void adc_poll_timer_callback(void* obj) {
  adc_poll();
}

//midi polling callback
static void midi_poll_timer_callback(void* obj) {
  // asynchronous, non-blocking read
  // UHC callback spawns appropriate events
  midi_read();
}

// monome polling callback
static void monome_poll_timer_callback(void* obj) {
  // asynchronous, non-blocking read
  // UHC callback spawns appropriate events
  ftdi_read();
}

// monome refresh callback
static void monome_refresh_timer_callback(void* obj) {
  //  if (monomeConnect) {
  //    print_dbg("\r\n posting monome refresh event");
  if(monomeFrameDirty > 0) {
    e.type = kEventMonomeRefresh;
    event_post(&e);
  }
  //  }
}

//----------------------------
//---- external functions

void init_app_timers(void) {
  timer_add(&screenTimer, 50, &screen_timer_callback, NULL );
  timer_add(&encTimer, 50, &enc_timer_callback, NULL );
}

// monome: start polling
void timers_set_monome(void) {
  print_dbg("\r\n setting monome timers");
  timer_add(&monomePollTimer, 	 20, &monome_poll_timer_callback, NULL );
  timer_add(&monomeRefreshTimer, 50, &monome_refresh_timer_callback, NULL );
}

// monome stop polling
void timers_unset_monome(void) {
  print_dbg("\r\n unsetting monome timers");
  timer_remove( &monomePollTimer );
  timer_remove( &monomeRefreshTimer ); 
}

// midi : start polling
void timers_set_midi(void) {
  print_dbg("\r\n setting midi timers");
  timer_add( &midiPollTimer, 20, &midi_poll_timer_callback, NULL );
  // TODO:
  //  timer_add(&midiRefreshTimer, eMidiRefreshTimerTag, 50,  &midi_refresh_timer_callback, NULL, 1);
}

// midi : stop polling
void timers_unset_midi(void) {
  print_dbg("\r\n unsetting midi timers");
  timer_remove( &midiPollTimer );
  // TODO:
  //  timer_remove(eMidiRefreshTimerTag); 
}

// adc : start polling
void timers_set_adc(u32 period) {
  print_dbg("\r\n setting adc timers, period: ");
  print_dbg_ulong(period);
  timer_add(&adcPollTimer, period, &adc_poll_timer_callback, NULL );
}

// adc : stop polling
void timers_unset_adc(void) {
  print_dbg("\r\n unsetting adc timers");
  timer_remove( &adcPollTimer );
} 

// change period of adc polling timer
void timers_set_adc_period(u32 period) {
  adcPollTimer.ticks = period;
}

// set custom callback
void timers_set_custom(softTimer_t* timer, u32 period, void* obj) {
  print_dbg("\r\n set custom timer, period: ");
  print_dbg_ulong(period);
  timer_add(timer, period, &app_custom_event_callback, obj );
}


// unset metro callback
void timers_unset_custom(softTimer_t* timer) {
  timer_remove(timer);
}
