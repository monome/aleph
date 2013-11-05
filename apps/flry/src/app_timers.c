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
static softTimer_t screenTimer = {.next = NULL };
/// static softTimer_t hidPollTimer  = {.next = NULL };

// poll monome device 
static softTimer_t monomePollTimer  = {.next = NULL };
// refresh monome device 
static softTimer_t monomeRefreshTimer  = {.next = NULL };

static softTimer_t lifeTimer  = {.next = NULL };

//----- callbacks

// screen refresh callback
static void screen_timer_callback(void* obj) {  
  render_update();
}

// hid poll timer callback
static void hid_poll_timer_callback(void* obj) {
  u8 i = 0;
  for(i=0; i<4; i++) {
    ctl_joy(i, joyVal[i]);
  } 
}

static event_t e;


static void life_timer_callback(void* obj) {
  life_update();
}

//====== external
void init_app_timers(void) {
  timer_add(&screenTimer, 20,  &screen_timer_callback, NULL);
  // timer_add(&hidPollTimer,        eHidPollTimerTag,        20,  &hid_poll_timer_callback,  1);
  timer_add(&lifeTimer, 100, &life_timer_callback, NULL);
 
}


/// FIXME: should do small optimizations.
// for example, can set/unset the sw timer 
// instead of always processing + conditional
// monome polling callback
static void monome_poll_timer_callback(void* obj) {
  //  if (monomeConnect > 0) {
  // start an ftdi transfer, callback handles event posting
  ftdi_read();
  //  }
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

void timers_set_monome(void) {
  print_dbg("\r\n setting monome timers");
  timer_add(&monomePollTimer, 20, &monome_poll_timer_callback, NULL);
  timer_add(&monomeRefreshTimer, 50, &monome_refresh_timer_callback, NULL);
}

void timers_unset_monome(void) {
  print_dbg("\r\n unsetting monome timers");
  timer_remove(&monomePollTimer);
  timer_remove(&monomeRefreshTimer); 
}
