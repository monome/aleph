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

// milliseconds to samples
#define MS_TO_SAMPS(x) ((x) * 48)

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
  static u32 interval;
  //    if(val > 0) {
      interval = sw_time(idx, val);
      //      print_dbg("\r\n press interval 0: ");
      //      print_dbg_ulong(interval);
      interval = MS_TO_SAMPS(interval);
      // bleh
      switch(idx) {
      case 0:
	ctl_param_change(eParam_delay0, MS_TO_SAMPS(interval));
	/// FIXME: weird wrapping behavior with these params, setting order, etc
	//	ctl_param_change(eParam_loop0,  MS_TO_SAMPS(interval) + 1);
	break;
      case 1:
	ctl_param_change(eParam_delay1, MS_TO_SAMPS(interval));
	//	ctl_param_change(eParam_loop1, MS_TO_SAMPS(interval) + 1);
	break;
      default:
	break;
      }
      //    }
}


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

extern void lppr_handler(event_t* ev) {
  switch (ev->eventType) {
  case kEventSwitch0:
    //    sw_post(0, ev->eventData);
    if(ev->eventData > 0) {
      sw_tap_delay(0, ev->eventData);
    }
    break;

  case kEventSwitch1:
    //sw_post(1, ev->eventData);    
    if(ev->eventData > 0) {
      sw_tap_delay(1, ev->eventData);
    }
    break;

  case kEventSwitch2:
    //    sw_post(2, ev->eventData);    
    break;

  case kEventSwitch3:
    //    sw_post(3, ev->eventData);    
    break;

  case kEventSwitch6:
    //   sw_post(6, ev->eventData);    
    break;

  case kEventSwitch7:
    //    sw_post(7, ev->eventData);    
    break;

  default:
    break;
  }
}
