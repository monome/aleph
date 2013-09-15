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
// static u32 swTicks[8][2];

//---------------------------------------------
//--------- static funcs

// process timing on key press and return interval
/* static u32 sw_time(u8 num, u8 val) { */
/*   u32 ret; */
/*   if( swTicks[num][val] > tcTicks) { */
/*     // overflow */
/*     ret = tcTicks + (0xffffffff - swTicks[num][val] ); */
/*     print_dbg("\r\n overflow in sw timer"); */
/*   } else { */
/*     ret = tcTicks - swTicks[num][val]; */
/*     print_dbg("\r\n sw_time: ");  */
/*     print_dbg_ulong(ret); */
/*   } */
/*   swTicks[num][val] = tcTicks; */
/*   return ret; */
/* } */

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
extern void dsyn_handler(event_t* ev) {
  u8 b;
  switch (ev->eventType) {
  case kEventSwitch0:
    b = ev->eventData > 0;
    ctl_set_gate(0, b);
    render_sw_on(0, b);
    break;
  case kEventSwitch1:
    render_sw_on(1, ev->eventData > 0);
    break;
  case kEventSwitch2:
    render_sw_on(2, ev->eventData > 0);
    break;
  case kEventSwitch3:
    render_sw_on(3, ev->eventData > 0);
    break;
    
  case kEventSwitch6:
    render_sw_on(2, ev->eventData > 0);
    break;
    
  case kEventSwitch7:
    render_sw_on(3, ev->eventData > 0);
    break;
    
  case kEventEncoder0:
    //    ctl_inc_dac(0, scale_knob_value(ev->eventData));
    break;
  case kEventEncoder1:
    //    ctl_inc_dac(1, scale_knob_value(ev->eventData));
    break;
  case kEventEncoder2:
    //    ctl_inc_dac(2, scale_knob_value(ev->eventData));
    break;
  case kEventEncoder3:
    //    ctl_inc_dac(3, scale_knob_value(ev->eventData));
    break;

  default:
    break;
  }
}
