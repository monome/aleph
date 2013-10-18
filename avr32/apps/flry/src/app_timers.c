//asf
#include "print_funcs.h"

// #include "monome.h"
#include "timers.h"
//#include "midi.h"
#include "app_timers.h"
#include "ctl.h"
#include "handler.h"
#include "render.h"
#include "monome.h"
#include "life.h"

//------ timers
// refresh the screen periodically
static swTimer_t screenTimer;
static swTimer_t hidPollTimer;

// poll monome device 
static swTimer_t monomePollTimer;
// refresh monome device 
static swTimer_t monomeRefreshTimer;
// poll midi device
//static swTimer_t midiPollTimer;

static swTimer_t lifeTimer;

//----- callbacks

// screen refresh callback
static void screen_timer_callback(int tag) {  
  render_update();
}

// hid poll timer callback
static void hid_poll_timer_callback(int tag) {
  u8 i = 0;
  for(i=0; i<4; i++) {
    ctl_joy(i, joyVal[i]);
  } 
}

static event_t e;


static void life_timer_callback(int tag) {
  life_update();
}


//====== external
void init_app_timers(void) {
  set_timer(&screenTimer,        eScreenTimerTag,        20,  &screen_timer_callback,  1);
  print_dbg("\r\n ALERT!!!!! hidPollTimer disabled. see line 55 app_timers.c");
  // set_timer(&hidPollTimer,        eHidPollTimerTag,        20,  &hid_poll_timer_callback,  1);
  // set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1);
  // set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 20,  &monome_refresh_timer_callback, 1);
  set_timer(&lifeTimer,   eLifeTimerTag, 100, &life_timer_callback, 1);
 
}


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

