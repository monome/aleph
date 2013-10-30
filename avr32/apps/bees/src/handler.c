/*
  handler.c
  bees

  app-specific UI event handler

 */

// asf
#include "gpio.h"
#include "delay.h"
#include "print_funcs.h"

// aleph-avr32
#include "aleph_board.h"
#include "event_types.h"

// bees
#include "handler.h"
#include "pages_protected.h"
#include "net_monome.h"
#include "pages.h"
#include "render.h"
#include "scene.h"

// alt-mode flag (momentary)
u8 altMode = 0;

/// gnarly enum hacks
static const eEventType kMenuEventMin = kEventEncoder0;
static const eEventType kMenuEventMax = kEventSwitch3;

// dummy handler
// static void handle_dummy(s32 data) { ;; }

// power-down handler
static void handle_powerdown(s32 data) {
  render_boot("powering down");
  render_boot("writing current scene to flash");
  scene_write_default();
  // power down
  delay_ms(100);
  gpio_clr_gpio_pin(POWER_CTL_PIN);
}

// array of handlers for all event types (!)


/// FIXME:
// shouldn't use. 
/// apps should define an array of function pointers for handlers.
void bees_handler(event_t* e) {
  const eEventType t = e->eventType;

  if( (t >= kMenuEventMin) && (t <= kMenuEventMax)) {
    // index FP 
    /* print_dbg("\r\n page handler: 0x"); */
    /* print_dbg_hex((u32) (curPage->handler[t - kMenuEventMin])); */
    curPage->handler[t - kMenuEventMin](e->eventData);
  } else {
    /// case 
    switch(t) {
    case kEventSwitch4: // mode
      // change mode
      break;
    case kEventSwitch5: // power      
      //      print_dbg("\r\n bees handler got power-switch event, value: ");
      //      print_dbg_hex(e->eventData);
      // write default scene...
      //      print_dbg("\r\n writing default scene to flash...");
      handle_powerdown(e->eventData);
      break;
    case kEventSwitch6: // FS 0
      // .. update op
      break;
    case kEventSwitch7: // FS 1
      // .. update op
      break;
    case kEventAdc0:
      // .. update op
      break;
    case kEventAdc1:
      // .. update op
      break;
    case kEventAdc2:
      // .. update op
      break;
    case kEventAdc3:
      // .. update op
      break;
    case kEventMonomeGridKey:
      // .. update ops
      break;
    case kEventMonomeGridTilt:
      // .. update ops
      break;
    case kEventMonomeRingEnc:
      // .. update ops
      break;
    case kEventMonomeRingKey:
      // .. update ops
      break;
    case kEventHidByte:
      // .. update ops
      break;
    case kEventMonomeConnect:
      // .. update monome focus
      break;
    case kEventMonomeDisconnect:
      // .. update monome focus 
      break;
    default:
      break;
    }
  }
}

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
