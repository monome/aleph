/*
  handler.c
  bees

  app-specific UI event handler

 */

#include "print_funcs.h"

#include "aleph_board.h"
#include "event_types.h"
#include "gpio.h"
#include "handler.h"
#include "menu_protected.h"
#include "net_monome.h"
#include "pages.h"

/// gnarly enum hacks
static const eEventType kMenuEventMin = kEventEncoder0;
static const eEventType kMenuEventMax = kEventSwitch3;



// mode flag
//static u8 keyMode = 0;


void bees_handler(event_t* e) {
  const eEventType t = e->eventType;
  
  print_dbg("\r\n bees handler, type: ");
  print_dbg_ulong(e->eventType);

  print_dbg(" , data: 0x");
  print_dbg_hex(e->eventData);
  

  /////// FIXME
  /// a nasty hack, relying on the relative values of enums ... :S

  //// truly it would be best for every application to define a handler (FP)
  /// for every UI event.

  if( (t >= kMenuEventMin) && (t <= kMenuEventMax)) {
    curPage->handler[t - kMenuEventMin](e->eventData);
  } else {
    switch(t) {
    case kEventSwitch6:
      // .. update op
      break;
    case kEventSwitch7:
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

  /*
  switch(e->eventType) {

    
    //----- function switches
  case kEventSwitch0:
    curPage->handler[ePageHandleKey0](e->eventData);
     break;
  case kEventSwitch1:
    curPage->handler[ePageHandleKey1](e->eventData);
     break;
  case kEventSwitch2:
    curPage->handler[ePageHandleKey2](e->eventData);
     break;
  case kEventSwitch3:
    curPage->handler[ePageHandleKey3](e->eventData);
     break;

    //----- foot switches
  case kEventSwitch6:
     break;
  case kEventSwitch7:
     break;

    //---- mode switch
  case kEventSwitch4:
    keyMode ^= 1;
    if(keyMode) { gpio_set_gpio_pin(LED_MODE_PIN); }
    else { gpio_clr_gpio_pin(LED_MODE_PIN); }
    //    menu_handleKey(eKeyMode, e->eventData);
    //// switch to play mode...
    break;
    
    //---- power switch
  case kEventSwitch5:
    break;

    //---- encoders    
  case kEventEncoder0:
    curPage->handler[ePageHandleEnc0](e->eventData);
    break;
  case kEventEncoder1:
    curPage->handler[ePageHandleEnc1](e->eventData);
    break;
  case kEventEncoder2:
    curPage->handler[ePageHandleEnc2](e->eventData);
    break;
  case kEventEncoder3:
    curPage->handler[ePageHandleEnc3](e->eventData);
    break;

    //=====  controllers
    //--- monome
  case kEventMonomeGridKey:
    (*monome_grid_key_handler)((void*)monomeOpFocus, (u32)e->eventData);
    break;
  case kEventMonomeGridTilt:
    break;
  case kEventMonomeRingEnc:
    break;
  case kEventMonomeRingKey:
    break;
    //--- midi
    // TODO
    ///---- HID
    // TODO

    //---- CV in
  case kEventAdc0:
    break;
  case kEventAdc1:
    break;
  case kEventAdc2:
    break;
  case kEventAdc3:
    break;
  default:
    break;
  }
    */
}

s32 scale_knob_value(s32 val) {
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

  if(val == 0) { return 0; }

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
