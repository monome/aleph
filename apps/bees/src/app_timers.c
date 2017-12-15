/* app_timers.c

   this is where callbacks are declared for all application-specific software timers.
   these callbacks are performed from the TC interrupt service routine.
   therefore, they should be kept small.

   avr32_lib defines a custom application event type, 
   which should be used when timer-based processing needs to be deferred to the main loop.
*/

//asf
#include "print_funcs.h"

// libavr32
#include "events.h"
#include "midi.h"
#include "monome.h"
#include "timers.h"

// aleph avr32
#include "app.h"
#include "adc_poll.h"
#include "control.h"
#include "encoders.h"

// bees
#include "ops/op_metro.h"

#include "app_timers.h"
#include "net_hid.h"
#include "net_poll.h"
#include "render.h"



//---------------------------
//---- static variables

//------ timers
// refresh screen
static softTimer_t screenTimer = { .next = NULL, .prev = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL, .prev = NULL };

// poll monome device 
static softTimer_t monomePollTimer = { .next = NULL, .prev = NULL };

// refresh monome device 
static softTimer_t monomeRefreshTimer  = { .next = NULL, .prev = NULL };

// poll midi device 
static softTimer_t midiPollTimer = { .next = NULL, .prev = NULL };

// refresh midi device 
/// TODO:
// static softTimer_t midiRefreshTimer;

// poll adc 
static softTimer_t adcPollTimer = { .next = NULL, .prev = NULL };

// poll hid
static softTimer_t hidPollTimer = { .next = NULL, .prev = NULL };

//--------------------------
//----- static functions

//----- callbacks

// the system defines a single event type for application events.
// event data is a pointer to an arbitrary object/
// here we use it for polled operators like op_metro.
static void app_custom_event_callback(void* obj) {
  // XXX look out - PD/beekeep has no event loop, so directly call
  // from the timer thread into the poll handler (not very threadsafe)
#ifdef BEEKEEP
  op_poll_t* op_poll = (op_poll_t*)obj;
  (*(op_poll->handler))(op_poll->op);
#else
  event_t e = { .type=kEventAppCustom, .data=(s32)obj };
  e.type = kEventAppCustom;
  // post the object's address in the event data field
  e.data = (s32)obj;
  event_post(&e);
#endif
}

// screen refresh callback
static void screen_timer_callback(void* obj) {
  // no! raise an event and do this in the main loop
  // (worth a try)
  //  render_update();
  event_t e = { .type = kEventScreenRefresh, .data = (s32)obj };
  event_post(&e);
  
}

// encoder accumulator polling callback
static void enc_timer_callback(void* obj) {
  static s16 val, valAbs;
  u8 i;
  event_t e;
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
  // blocking read
  adc_poll();
  // spawn events here
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
#if BEEKEEP
#else
  ftdi_read();
#endif
}

// monome refresh callback
static void monome_refresh_timer_callback(void* obj) {
  if(monomeFrameDirty > 0) {
    event_t e = { .type = kEventMonomeRefresh };
    event_post(&e);
  }
}

// hid polling callback
static void hid_poll_timer_callback(void* obj) {
#if BEEKEEP
#else
  net_handle_hid_packet();
#endif
}

//----------------------------
//---- external functions

// order matters here...
void init_app_timers(void) {
  timer_add(&screenTimer, 50, &screen_timer_callback, NULL );
  timer_add(&encTimer, 50, &enc_timer_callback, NULL );
}

// monome: start polling
void timers_set_monome(void) {
  timer_add(&monomePollTimer, 	 1, &monome_poll_timer_callback, NULL );
  timer_add(&monomeRefreshTimer, 50, &monome_refresh_timer_callback, NULL );
}

// monome stop polling
void timers_unset_monome(void) {
  timer_remove( &monomePollTimer );
  timer_remove( &monomeRefreshTimer ); 
}

// midi : start polling
void timers_set_midi(void) {
  /// FIXME: where should default periods be defined...
  timer_add( &midiPollTimer, 1, &midi_poll_timer_callback, NULL );
  // TODO??
  //  timer_add(&midiRefreshTimer, eMidiRefreshTimerTag, 50,  &midi_refresh_timer_callback, NULL, 1);
}

// midi : stop polling
void timers_unset_midi(void) {
  timer_remove( &midiPollTimer );
  // TODO??
  //  timer_remove(eMidiRefreshTimerTag); 
}

// adc : start polling
void timers_set_adc(u32 period) {
  timer_add(&adcPollTimer, period, &adc_poll_timer_callback, NULL );
}

// adc : stop polling
void timers_unset_adc(void) {
  timer_remove( &adcPollTimer );
} 

// change period of adc polling timer
void timers_set_adc_period(u32 period) {
  adcPollTimer.ticks = period;
}


// hid : start polling
void timers_set_hid(void) {
  timer_add(&hidPollTimer, 20, &hid_poll_timer_callback, NULL );
}

// hid : stop polling
void timers_unset_hid(void) {
  timer_remove( &hidPollTimer );
} 

// change period of hid polling timer
void timers_set_hid_period(u32 period) {
  hidPollTimer.ticks = period;
}

// set custom callback
// the callback just pushes timer interrupts back into the event queue.
// this is to keep network processing out of the timer IRQ itself...
void timers_set_custom(softTimer_t* timer, u32 period, void* obj) {
  timer_add(timer, period, &app_custom_event_callback, obj );
}

// XXX hack alert!

// a) we define the time units for BEES to be 2ms, allowing up to a
// minute or so of audio to be indexed using 16bit number.

// b) the timing of libavr32 (in milliseconds) seems always off a bit.
// Measured timings & added a weird correction which is tested & gives
// pretty tight timing on my aleph (RV)

// c) obviously the two functions below are not mutually inverse maps,
// but they should be close enough for general use!

s32 timers_2ms_tick_to_libavr32_tick (s16 ticks_2ms) {
  s32 ret = ticks_2ms;
  ret = ret << 1; // 2ms clock, not 1ms

  // timing fudge-factor
  ret -= ret >> 7;
  ret -= ret >> 8;
  ret -= ret >> 9;
  return ret;
}

s16 timers_libavr32_tick_to_2ms_tick (s32 ticks_libavr32) {
  s32 ret = ticks_libavr32;
  ret += ret >> 7;
  ret += ret >> 8;
  ret += ret >> 9;
  ret = ret >> 1;

  //clip to s16 for extra paranoia
  if (ret >= 32767) {
    return 32767;
  }
  else if (ret <= -32768) {
    return -32768;
  }
  else {
    return ret;
  }
}


/* // unset metro callback */
/* void timers_unset_custom(softTimer_t* timer) { */
/*   timer_remove(timer); */
/* } */
