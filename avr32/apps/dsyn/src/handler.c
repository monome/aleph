/*
  handler.c
  dsyn
  aleph-avr32

 */

//asf
#include "print_funcs.h"
#include "gpio.h"
// avr32
#include "aleph_board.h"
#include "control.h"
#include "event_types.h"
#include "interrupts.h"
#include "screen.h"
// dsyn
#include "app_event_types.h"
#include "app_timers.h"
#include "ctl.h"
#include "files.h"
 #include "grid.h"
 #include "handler.h"
 #include "monome.h"
 #include "render.h"
 #include "sequence.h"

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
  // exponential table
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


static s32 scale_knob_value_fast(s32 val) {
  return scale_knob_value(val);
}


 static void handle_monome_connect(u32 data) {
   eMonomeDevice dev;
   u8 w;
   u8 h;
   monome_connect_parse_event_data(data, &dev, &w, &h);
   if(dev != eDeviceGrid) {
     print_dbg("\r\n DSYN monome connect: unsupported device");
     return;
   }
   print_dbg("\r\n DSYN: connecting grid device");
   grid_set_size(w, h);
   timers_set_monome();
 }

 static void handle_sw(u8 id, u8 b) {
     //    ctl_set_gate(2, b);
     render_sw_on(id, b);
     if(b) {
       ctl_set_voice_select(id);
     }			   
     //    seq_tog_stage(2, seq_get_pos());
 }

 //---------------------------------------
 //---- external funcs

 // handle key presses
 extern void dsyn_handler(event_t* ev) {
   //  u8 b;

   //  print_dbg("\r\n app event handler: ");
   //  print_dbg_hex(ev->eventType);

   switch (ev->eventType) {

   case kEventSeqNext:
     seq_advance();
     break;

   case kEventSwitch0:
     handle_sw(0, ev->eventData > 0);
     break;
   case kEventSwitch1:
     handle_sw(1, ev->eventData > 0);
     break;
   case kEventSwitch2:
     handle_sw(2, ev->eventData > 0);
     break;
   case kEventSwitch3:
     handle_sw(3, ev->eventData > 0);
     break;

   case kEventSwitch4:
     // mode button: write config file
     if(ev->eventData > 0) {
       print_dbg("\r\n write params...");
       files_write_params();
       /* grid_toggle_edit_mode(); */
       /* if(gpio_get_pin_value(LED_MODE_PIN)) { */
       /* 	gpio_clr_gpio_pin(LED_MODE_PIN); */
       /* } else { */
       /* 	gpio_set_gpio_pin(LED_MODE_PIN); */
       /* } */
     }
     break;

   case kEventSwitch6:
     render_sw_on(2, ev->eventData > 0);
     break;

   case kEventSwitch7:
     render_sw_on(3, ev->eventData > 0);
     break;

   case kEventMonomeGridKey:
     grid_handle_key_event(ev->eventData);
     break;

   case kEventEncoder0:
     /// TEMPO
     ctl_inc_tempo(scale_knob_value(ev->eventData));
     break;
  case kEventEncoder1:
    // SCROLL GRID
    grid_inc_scroll( ev->eventData > 0 ? 1 : -1);
    break;
  case kEventEncoder2:
    // PARAM VALUE
    ctl_inc_param( scale_knob_value_fast(ev->eventData) );
    break;
  case kEventEncoder3:
    // PARAM IDX 
    ctl_inc_param_select( ev->eventData > 0 ? 1 : -1 );
    break;
  case kEventMonomeConnect :
    handle_monome_connect((u32)ev->eventData);
    break;
  default:
    break;
  }
}
