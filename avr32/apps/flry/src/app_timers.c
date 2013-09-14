//asf
#include "print_funcs.h"

// #include "monome.h"
#include "timers.h"
//#include "midi.h"
#include "app_timers.h"
#include "render.h"

//------ timers
// refresh the screen periodically
static swTimer_t screenTimer;
// poll adcs 
//static swTimer_t adcPollTimer;

// poll monome device 
//static swTimer_t monomePollTimer;
// refresh monome device 
//static swTimer_t monomeRefreshTimer;
// poll midi device
//static swTimer_t midiPollTimer;

static swTimer_t metroTimer;

//----- callbacks

// screen refresh callback
static void screen_timer_callback(int tag) {  
  render_update();
  /* if(refresh) { */
  /*   e.eventType = kEventRefresh; */
  /*   post_event(&e); */
  /*   refresh = 0; */
  /* } */
}

// metronome timer callback
static void metro_timer_callback(int tag) {
  print_dbg(">");
}

/////////////////////////////
//////////////
//////  not using these:


// adc polling callback
/* static void adc_timer_callback(int tag) { */
/*   adc_poll(); */
/* } */

/* // monome polling callback */
/* static void monome_poll_timer_callback(int tag) { */
/*   if (monomeConnect > 0) { */
/*     // start an ftdi transfer, callback handles event posting */
/*     ftdi_read(); */
/*   } */
/* } */

// monome refresh callback
/* static void monome_refresh_timer_callback(int tag) { */
/*   if (monomeConnect) { */
/*     //    print_dbg("\r\n posting monome refresh event"); */
/*     if(monomeFrameDirty > 0) { */
/*       e.eventType = kEventMonomeRefresh; */
/*       post_event(&e); */
/*     } */
/*   } */
/* } */

// midi polling callback
/* static void midi_poll_timer_callback(int tag) { */
/*   if(midiConnect) { */
/*     midi_read(); */
/*   } */
/* } */


//====== external
void init_app_timers(void) {
  set_timer(&screenTimer,        eScreenTimerTag,        20,  &screen_timer_callback,  1);
  //  set_timer(&adcTimer,           eAdcTimerTag,           5,   &adc_timer_callback,     1);
  //  set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1);
  //  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 20,  &monome_refresh_timer_callback, 1);
  //  set_timer(&midiPollTimer,      eMidiPollTimerTag,      5,  &midi_poll_timer_callback, 1);
  set_timer(&metroTimer,        eMetroTimerTag,        100,  &metro_timer_callback,  1);

}
