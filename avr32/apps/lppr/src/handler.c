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
#include "ctl.h"
#include "render.h"

// up and down interval timers for keys and footswitches
static u32 swTicks[8][2];

// keep track of the last touched UI element (when appropriate)
static eEventType touched = kNumEvents;

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
  ctl_set_delay_ms(idx,  sw_time(idx, val) );
}
//--------------------------
//--- static func def

// return param increment given encoder ticks (knob acceleration)
static fix16 scale_knob_value(const s32 v) {
  s32 vabs = BIT_ABS(v);
  //  print_dbg("\r\n knob acc, val: ");
  //  print_dbg_hex((u32)v);
  if(vabs < 4) {
    //    print_dbg("\r\n ");
    return v;
  } else if (vabs < 8) {
    //    print_dbg("\r\n knob acc 1");
    return v << 2;
  } else if (vabs < 12) {
    //    print_dbg("\r\n knob acc 2");
    return v << 4;
  } else if (vabs < 19) {
    //    print_dbg("\r\n knob acc 3");
    return v << 5;
  } else if (vabs < 25) {
    //    print_dbg("\r\n knob acc 4");
    return v << 6;  } 
  else if (vabs < 32) {
    //    print_dbg("\r\n knob acc 4");
    return v << 6;
  } else {
    //    print_dbg("\r\n knob acc max");
    return v << 12;
  }
}


//---------------------------------------
//---- external funcs

// handle key presses
extern void lppr_handler(event_t* ev) {
  u8 touchedThis = 0;
  // clear the main region if this is a new touch
  print_dbg("\r\n \r\n check type: ");
  print_dbg_ulong(ev->eventType);
  if(touched != ev->eventType) {
    print_dbg("\r\n new touch");
    touchedThis = 1;
    touched = ev->eventType;
  }
  switch (ev->eventType) {
  case kEventSwitch0:
    /// tap delaytime 1
    if(ev->eventData > 0) {
      if(touchedThis) {
	render_touched_delaytime(0);
      }
      sw_tap_delay(0, ev->eventData);
    }
    render_sw_on(0, ev->eventData > 0);
    break;

  case kEventSwitch1:
    // tap delaytime 2
    if(ev->eventData > 0) {
      if(touchedThis) {
	render_touched_delaytime(1);
      }	
      sw_tap_delay(1, ev->eventData);
    }
    render_sw_on(1, ev->eventData > 0);
    break;

  case kEventSwitch2:
    if(ev->eventData > 0) {
      if(touchedThis) {
	//	render_new_param_touch();
      }
      // record loop on line 1
      ctl_loop_record(1);
    } 
    render_sw_on(2, ev->eventData > 0);
    break;

  case kEventSwitch3:
    if(ev->eventData > 0) {
      if(touchedThis) {
	//	render_new_param_touch();
      }
      // record loop on line 2
      ctl_loop_playback(1);
    }
    render_sw_on(3, ev->eventData > 0);
    break;
    
  case kEventSwitch6:
    if(ev->eventData > 0) {
      ctl_loop_record(1);
    } 
    break;
    
  case kEventSwitch7:
    if(ev->eventData > 0) {
      ctl_loop_playback(1);
    }
    break;
    
  case kEventEncoder0:
    if(touchedThis) {
      render_touched_fb(0);
    }
    ctl_inc_fb(0, scale_knob_value(ev->eventData));
    break;

  case kEventEncoder1:
    if(touchedThis) {
      render_touched_mix(0);
    }
    ctl_inc_mix(0, scale_knob_value(ev->eventData));
    break;

  case kEventEncoder2:
    if(touchedThis) {
      render_touched_freq(0);
    }
    ctl_inc_freq(0, scale_knob_value(ev->eventData));
    break;

  case kEventEncoder3:
    if(touchedThis) {
      render_touched_res(0);
    }
    ctl_inc_res(0, scale_knob_value(ev->eventData));
    break;

  default:
    break;
  }
}
