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
static s32 scale_knob_value(s32 val) {
  static const u32 kNumKnobScales_1 = 23;
  static const u32 knobScale[24] = {
    0x00000001,
    0x00000005,
    0x0000000C,
    0x0000001C,
    0x00000041,
    0x00000098,
    0x0000015F,
    0x0000032C,
    0x00000756,
    0x000010F3,
    0x0000272B,
    0x00005A82,
    0x0000D124,
    0x0001E343,
    0x00045CAE,
    0x000A1451,
    0x00174A5A,
    0x0035D13F,
    0x007C5B28,
    0x011F59AC,
    0x0297FB5A,
    0x05FE4435,
    0x0DD93CDC,
    0x1FFFFFFD,
  };

  s32 vabs = BIT_ABS(val);
  s32 ret = val;
  if(vabs > kNumKnobScales_1) {
    vabs = kNumKnobScales_1;
  }
  ret = knobScale[vabs];
  if(val < 0) {
    ret = BIT_NEG_ABS(ret);
  }
  return ret;
}

//---------------------------------------
//---- external funcs

// handle key presses
extern void flry_handler(event_t* ev) {
  switch (ev->eventType) {
  case kEventSwitch0:
    // display
    render_sw_on(0, ev->eventData > 0);
    break;
  case kEventSwitch1:
    // display
    render_sw_on(1, ev->eventData > 0);
    break;
  case kEventSwitch2:
    // display
    render_sw_on(2, ev->eventData > 0);
    break;
  case kEventSwitch3:
    // display
    render_sw_on(3, ev->eventData > 0);
    break;
    
  case kEventSwitch6:
    render_sw_on(2, ev->eventData > 0);
    break;
    
  case kEventSwitch7:
    render_sw_on(3, ev->eventData > 0);
    break;

  case kEventHidByte:
    print_dbg("\r\n received HID byte, index: ");
    print_dbg_ulong( (ev->eventData & 0x0000ff00) >> 8);
    print_dbg(", value: ");
    print_dbg_hex( (ev->eventData & 0x000000ff));
    break;

  case kEventEncoder0:
    ctl_inc_dac(0, scale_knob_value(ev->eventData));
    break;
  case kEventEncoder1:
    ctl_inc_dac(1, scale_knob_value(ev->eventData));
    break;
  case kEventEncoder2:
    ctl_inc_dac(2, scale_knob_value(ev->eventData));
    break;
  case kEventEncoder3:
    ctl_inc_dac(3, scale_knob_value(ev->eventData));
    break;

  default:
    break;
  }
}
