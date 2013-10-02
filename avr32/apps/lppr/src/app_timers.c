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
#include "app_timers.h"
#include "ctl.h"
#include "control.h"
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

// poll adcs 
//static swTimer_t adcPollTimer;

// poll midi device
//static swTimer_t midiPollTimer;


//----- callbacks

// screen refresh callback
static void screen_timer_callback(int tag) {  
  render_update();
}


// TODO: connecting CV to filter parameters would be good
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
  //  set_timer(&midiPollTimer,      eMidiPollTimerTag,      5,  &midi_poll_timer_callback, 1);
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



/* #if 0 */


/* #include "print_funcs.h" */

/* #include "monome.h" */
/* #include "timers.h" */
/* //#include "midi.h" */
/* #include "app_timers.h" */
/* #include "render.h" */

/* ///// test */
/* #include "region.h" */
/* ///// */

/* //------ timers */
/* // refresh the screen periodically */
/* static swTimer_t screenTimer; */
/* // poll adcs  */
/* //static swTimer_t adcPollTimer; */

/* // poll monome device  */
/* //static swTimer_t monomePollTimer; */
/* // refresh monome device  */
/* //static swTimer_t monomeRefreshTimer; */
/* // poll midi device */
/* //static swTimer_t midiPollTimer; */

/* // poll adcs  */
/* static swTimer_t test; */


/* //----- callbacks */

/* // screen refresh callback */
/* static void screen_timer_callback(int tag) {   */
/*   render_update(); */
/*   /\* if(refresh) { *\/ */
/*   /\*   e.eventType = kEventRefresh; *\/ */
/*   /\*   post_event(&e); *\/ */
/*   /\*   refresh = 0; *\/ */
/*   /\* } *\/ */
/* } */


/* static void test_callback(int tag) { */
/*   static char testr[] = { 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46}; */
/*   u8 i; */
/*   render_test(testr); */
/*   for(i=0; i<14; i++) {  */
/*     testr[i]++; */
/*     if(testr[i] > 125) { testr[i] = 33; } */
/*   } */
/*   //  print_dbg("\r\n"); */
/*   //  print_dbg(testr); */
/* } */

/* ///////////////////////////// */
/* ////////////// */
/* //////  not using these: */


/* // adc polling callback */
/* /\* static void adc_timer_callback(int tag) { *\/ */
/* /\*   adc_poll(); *\/ */
/* /\* } *\/ */

/* /\* // monome polling callback *\/ */
/* /\* static void monome_poll_timer_callback(int tag) { *\/ */
/* /\*   if (monomeConnect > 0) { *\/ */
/* /\*     // start an ftdi transfer, callback handles event posting *\/ */
/* /\*     ftdi_read(); *\/ */
/* /\*   } *\/ */
/* /\* } *\/ */

/* // monome refresh callback */
/* /\* static void monome_refresh_timer_callback(int tag) { *\/ */
/* /\*   if (monomeConnect) { *\/ */
/* /\*     //    print_dbg("\r\n posting monome refresh event"); *\/ */
/* /\*     if(monomeFrameDirty > 0) { *\/ */
/* /\*       e.eventType = kEventMonomeRefresh; *\/ */
/* /\*       post_event(&e); *\/ */
/* /\*     } *\/ */
/* /\*   } *\/ */
/* /\* } *\/ */

/* // midi polling callback */
/* /\* static void midi_poll_timer_callback(int tag) { *\/ */
/* /\*   if(midiConnect) { *\/ */
/* /\*     midi_read(); *\/ */
/* /\*   } *\/ */
/* /\* } *\/ */


/* //====== external */
/* void init_app_timers(void) { */
/*   set_timer(&screenTimer,        eScreenTimerTag,        20,  &screen_timer_callback,  1); */
/*   set_timer(&test,        eTestTag,       100,  &test_callback,  1); */
/*   //  set_timer(&adcTimer,           eAdcTimerTag,           5,   &adc_timer_callback,     1); */
/*   //  set_timer(&monomePollTimer,    eMonomePollTimerTag,    20,  &monome_poll_timer_callback,    1); */
/*   //  set_timer(&monomeRefreshTimer, eMonomeRefreshTimerTag, 20,  &monome_refresh_timer_callback, 1); */
/*   //  set_timer(&midiPollTimer,      eMidiPollTimerTag,      5,  &midi_poll_timer_callback, 1); */
/* } */

/* void kill_test(void) { */
/*   kill_timer(eTestTag); */
/* } */



/* #endif */
