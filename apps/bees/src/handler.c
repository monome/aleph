/*
  handler.c
  bees

  app-specific UI event handlers.
  
  app.c defines a global array of function pointers to handle system events.
  main.c defines its own handlers, most of which don't do anything.
  some events (e.g. ftdiConnect, monomePoll) call pretty low-level driver stuff,
  and there shouldn't be any need for bees or other applications to customize them.

  handlers that do need to be customized go here.
  define static functions and then populate the global array with pointers to these functions.
  this step should happen in e.g. app_launch().

*/

// asf
#include "gpio.h"
#include "delay.h"
#include "print_funcs.h"

// aleph-avr32
#include "aleph_board.h"
#include "app.h"
#include "event_types.h"

// bees
#include "app_timers.h"
#include "handler.h"
#include "net_midi.h"
#include "net_monome.h"
#include "net_protected.h"
#include "pages.h"
#include "render.h"
#include "scene.h"

///-------------------------------------
///---- static event handlers


static void handler_Adc0(s32 data) { 
  // update ADC system operator
  /* print_dbg("\r\n bees handle adc0 event, data: 0x"); */
  /* print_dbg_hex(data); */
  net_activate(net_op_in_idx(opSysAdcIdx, 2), data, NULL);
}

static void handler_Adc1(s32 data) { 
  // update ADC system operator
  /* print_dbg("\r\n bees handle adc0 event, data: 0x"); */
  /* print_dbg_hex(data); */
  net_activate(net_op_in_idx(opSysAdcIdx, 3), data, NULL);

}

static void handler_Adc2(s32 data) { 
  // update ADC system operator
  /* print_dbg("\r\n bees handle adc0 event, data: 0x"); */
  /* print_dbg_hex(data); */
  net_activate(net_op_in_idx(opSysAdcIdx, 4), data, NULL);

}

static void handler_Adc3(s32 data) { 
  // update ADC system operator
  /* print_dbg("\r\n bees handle adc0 event, data: 0x"); */
  /* print_dbg_hex(data); */
  net_activate(net_op_in_idx(opSysAdcIdx, 5), data, NULL);
}

// function key and encoder handlers are page-specific
/* static void handler_Encoder0(s32 data) {  */
/*   /// ech, this is wrong... */
/*   curPage->handler[kEventEncoder0](data); */
/* } */

/* static void handler_Encoder1(s32 data) {  */
/*   curPage->handler[kEventEncoder1](data); */
/* } */

/* static void handler_Encoder2(s32 data) {  */
/*   curPage->handler[kEventEncoder2](data); */
/* } */

/* static void handler_Encoder3(s32 data) {  */
/*   curPage->handler[kEventEncoder3](data); */
/* } */

/* static void handler_Switch0(s32 data) {  */
/*   curPage->handler[kEventSwitch0](data); */
/* } */

/* static void handler_Switch1(s32 data) {  */
/*   curPage->handler[kEventSwitch1](data); */
/* } */

/* static void handler_Switch2(s32 data) {  */
/*   curPage->handler[kEventSwitch2](data); */
/* } */

/* static void handler_Switch3(s32 data) {  */
/*   curPage->handler[kEventSwitch3](data); */
/* } */

static void handler_Switch4(s32 data) { 
  // mode switch
  if(data > 0) {
    if(pages_toggle_play()) {
      gpio_set_gpio_pin(LED_MODE_PIN);
    } else {
      gpio_clr_gpio_pin(LED_MODE_PIN);
    }
  }
}

static void handler_Switch5(s32 data) { 
  /// power switch
  render_boot("");
  render_boot("");
  render_boot("");
  render_boot("");
  render_boot("");
  render_boot("");
  render_boot("powering down");
  render_boot("writing current scene to flash");
  scene_write_default();
  // power down
  delay_ms(100);
  gpio_clr_gpio_pin(POWER_CTL_PIN);
}

static void handler_Switch6(s32 data) {
  // footswitch 1
  net_activate(net_op_in_idx(opSysSwIdx[4], 0), data, NULL);
}

static void handler_Switch7(s32 data) { 
  // footswitch 2
  net_activate(net_op_in_idx(opSysSwIdx[5], 0), data, NULL);
} 

static void handler_MonomeConnect(s32 data) { 
  timers_set_monome();
}

static void handler_MonomeDisconnect(s32 data) { 
  timers_unset_monome();
}

static void handler_MonomeGridKey(s32 data) { 
  // net_monome.c defines a dynamic pointer to a single grid event handler.
  // this is so bees can arbitrate focus between multiple grid ops.
  (*monome_grid_key_handler)(monomeOpFocus, data);
}

static void handler_MonomeGridTilt(s32 data) { 
  // TODO: update ops
}

static void handler_MonomeRingEnc(s32 data) {
  // TODO: update ops 
}

static void handler_MonomeRingKey(s32 data) { 
  // TODO: update ops
}

static void handler_MidiConnect(s32 data) {
  timers_set_midi();
}

static void handler_MidiDisconnect(s32 data) { 
  timers_unset_midi();
}

static void handler_MidiPacket(s32 data) {
  net_handle_midi_packet(data);
}

static void handler_HidConnect(s32 data) {
  // nothing to do... ?
}

static void handler_HidDisconnect(s32 data) {
  // nothing to do... ?
}

static void handler_HidByte(s32 data) {
  // TODO: update ops
}

//-------------------------------------
//---- extern

/// more maintainable, perhaps, to explicitly assign these...
/// this way the order of the event types enum doesn't matter.
void assign_bees_event_handlers(void) {
  app_event_handlers[ kEventAdc0 ]	= &handler_Adc0 ;
  app_event_handlers[ kEventAdc1 ]	= &handler_Adc1 ;
  app_event_handlers[ kEventAdc2 ]	= &handler_Adc2 ;
  app_event_handlers[ kEventAdc3 ]	= &handler_Adc3 ;
  // power/mode/footswitches here, fn switches in page handlers
  app_event_handlers[ kEventSwitch4 ]	= &handler_Switch4 ;
  app_event_handlers[ kEventSwitch5 ]	= &handler_Switch5 ;
  app_event_handlers[ kEventSwitch6 ]	= &handler_Switch6 ;
  app_event_handlers[ kEventSwitch7 ]	= &handler_Switch7 ;
  app_event_handlers[ kEventMonomeConnect ]	= &handler_MonomeConnect ;
  app_event_handlers[ kEventMonomeDisconnect ]	= &handler_MonomeDisconnect ;
  app_event_handlers[ kEventMonomeGridKey ]	= &handler_MonomeGridKey ;
  app_event_handlers[ kEventMonomeGridTilt ]	= &handler_MonomeGridTilt ;
  app_event_handlers[ kEventMonomeRingEnc ]	= &handler_MonomeRingEnc ;
  app_event_handlers[ kEventMonomeRingKey ]	= &handler_MonomeRingKey ;
  app_event_handlers[ kEventMidiConnect ]	= &handler_MidiConnect ;
  app_event_handlers[ kEventMidiDisconnect ]	= &handler_MidiDisconnect ;
  app_event_handlers[ kEventMidiPacket ]	= &handler_MidiPacket ;
  app_event_handlers[ kEventHidConnect ]	= &handler_HidConnect ;
  app_event_handlers[ kEventHidDisconnect ]	= &handler_HidDisconnect ;
  app_event_handlers[ kEventHidByte ]	= &handler_HidByte ;
}

//------------------------
//--- knob scaling

// full-scale
s32 scale_knob_value(s32 val) {
  static const u32 kNumKnobScales_1 = 23;
  static const u32 knobScale[24] = {
    ///--- 3 linear segments:
    // slope = 1
    0x00000001, // 1
    0x00000002, // 2
    // slope = 0x10
    0x00000030, // 3
    0x00000030, // 4
    0x00000040, // 5
    0x00000050, // 6
    0x00000060, // 7
    0x00000070, // 8
    0x00000080, // 9
    0x00000090 ,  // 10
    0x000000a0 , // 11
    0x000000b0 , // 12
    // slope = 0x100
    0x00000c00 , // 13
    0x00000d00 , // 14
    0x00000e00 , // 15
    0x00000f00 , // 16
    0x00001000 , // 17
    0x00001100 , // 18
    0x00001200 , // 19
    0x00001300 , // 20
    0x00001400 , // 21
    0x00001500 , // 22
    // ultra fast
    0x10000000 , // 23
    0x20000000 , // 24
  };
  s32 vabs = BIT_ABS(val);
  s32 ret = val;

  if(vabs > kNumKnobScales_1) {
    vabs = kNumKnobScales_1;
  }
  ret = knobScale[vabs - 1];
  if(val < 0) {
    ret = BIT_NEG_ABS(ret);
  }
  return ret;
}


// lower slope
s32 scale_knob_value_small(s32 val) {
  static const u32 kNumKnobScales_1 = 23;
  static const u32 knobScale[24] = {
    ///--- 3 linear segments:
    // slope = 1
    0x00000001, // 1
    0x00000002, // 2
    // slope = 0x10
    0x00000030, // 3
    0x00000030, // 4
    0x00000040, // 5
    0x00000050, // 6
    0x00000060, // 7
    0x00000070, // 8
    0x00000080, // 9
    0x00000090 ,  // 10
    0x000000a0 , // 11
    0x000000b0 , // 12
    0x000000c0 , // 13
    0x000000d0 , // 14
    0x000000e0 , // 15
    0x000000f0 , // 16
    // slope == 0x100
    0x00000100 , // 17
    0x00000200 , // 18
    0x00000300 , // 19
    0x00000400 , // 20
    0x00000500 , // 21
    0x00000600 , // 22
    0x00000700 , // 23
    0x00000800 , // 24
  };

  s32 vabs = BIT_ABS(val);
  s32 ret = val;

  if(vabs > kNumKnobScales_1) {
    vabs = kNumKnobScales_1;
  }
  ret = knobScale[vabs - 1];
  if(val < 0) {
    ret = BIT_NEG_ABS(ret);
  }
  return ret;
}
