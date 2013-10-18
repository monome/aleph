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

  /* print_dbg("\r\n --- "); */
  /* print_dbg("\r\n bees handler, type: "); */
  /* print_dbg_ulong(e->eventType); */

  /* print_dbg(" , data: 0x"); */
  /* print_dbg_hex(e->eventData); */
  
  /////// FIXME
  /// a nasty hack, relying on the relative values of enums ... :S

  //// truly it would be best for every application to define a handler (FP)
  /// for every UI event:
  //  (*(curPage->handler[e->eventType]))(e->eventData);


  if( (t >= kMenuEventMin) && (t <= kMenuEventMax)) {
    // index FP 
    curPage->handler[t - kMenuEventMin](e->eventData);
  } else {
    /// case 
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
    /// linear segment
    1, // 0
    2, // 1
    3, // 2
    4, // 3
    // hyper exponential segment
    0x00000008, // 4
    0x00000010, // 5
    0x00000100, // 6
    // linear
    0x00001000, // 7
    0x00002000, // 8
    0x00003000 ,  // 9
    0x00004000 , // 10
    0x00005000 , // 11
    0x00006000 , // 12
    0x00007000 , // 13
    0x00008000 , // 14
    0x00009000 , // 15
    0x0000a000 , // 16
    0x0000b000 , // 17
    // exponential
    0x0000c000 , // 18
    0x00018000 , // 19
    0x00030000 , // 20
    0x00060000 , // 21
    0x000C0000 , // 22
    0x00180000 , // 23
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
