/*
  ui.c
  lppr
  aleph-avr32

 */

//asf
#include "print_funcs.h"

// avr32
#include "control.h"
#include "event_types.h"
#include "interrupts.h"
#include "screen.h"
// lppr
#include "handler.h"
#include "params.h"
#include "renderer.h"

// names of switches
//static char * swName[] = { "key1", "key2","key3","key4", "mode", "power", "foot1", "foot2" };

// up and down interval timers for keys and footswitches
static u32 swTicks[8][2];


//---------------------------------------------
//--------- static funcs

// process timing on key press and return interval
static u32 sw_time(u8 num, u8 val) {
  u32 ret;
  if( swTicks[num][val] > tcTicks) {
    // overflow
    ret = tcTicks + (0xffffffff - swTicks[num][val] );
    print_dbg("\r\n overflow in sw timer");
  } else {
    ret = tcTicks - swTicks[num][val];
    print_dbg("\r\n sw_time: "); 
    print_dbg_ulong(ret);
  }
  swTicks[num][val] = tcTicks;
  return ret;
}

// set delay time from switch tap
static void sw_tap_delay(u8 idx, u8 val) {
  //  static u32 interval;
  //    if(val > 0) {
  //  interval =;
      param_set_delay_ms(idx,  sw_time(idx, val) );
      //      print_dbg("\r\n press interval 0: ");
      //      print_dbg_ulong(interval);
}

// start recording loop in delayline 1
/* static void loop_record(void) { */
/*   param_set_loop_record(); */
/* } */


/* // stop recording and begin loop playback */
/* static void loop_playback(void) { */
/* } */


/* static void sw_post(u8 num, u8 val) { */
/*    screen_line(0, 0, swName[num], 0xf); */
/*    print_dbg("\r\n"); print_dbg(swName[num]); */
/*     if(val > 0) { */
/*       screen_line(20, 0, "down", 0xf); */
/*       print_dbg(" down"); */
/*     } else { */
/*       screen_line(20, 0, "up", 0xf); */
/*       print_dbg(" up"); */
/*     } */
/* } */

//---------------------------------------
//---- external funcs

// handle key presses
extern void lppr_handler(event_t* ev) {
  switch (ev->eventType) {
  case kEventSwitch0:
    /// tap delaytime 1
    if(ev->eventData > 0) {
      sw_tap_delay(0, ev->eventData);
    }

    render_sw_on(0, ev->eventData > 0);
    break;

  case kEventSwitch1:
    // tap delaytime 2
    if(ev->eventData > 0) {
      sw_tap_delay(1, ev->eventData);
    }
    render_sw_on(1, ev->eventData > 0);
    break;

  case kEventSwitch2:
    if(ev->eventData > 0) {
      // record loop on line 1
      param_loop_record(1);
    } 
    render_sw_on(2, ev->eventData > 0);
    break;

  case kEventSwitch3:
    if(ev->eventData > 0) {
      // record loop on line 2
      param_loop_playback(1);
    }
    render_sw_on(3, ev->eventData > 0);
    break;

  case kEventSwitch6:
    if(ev->eventData > 0) {
      param_loop_record(1);
    } 
    break;

  case kEventSwitch7:
    if(ev->eventData > 0) {
      param_loop_playback(1);
    }
    break;


  case kEventEncoder0:
    break;
  case kEventEncoder1:
    break;
  case kEventEncoder2:
    break;
  case kEventEncoder3:
    break;


  default:
    break;
  }
}
