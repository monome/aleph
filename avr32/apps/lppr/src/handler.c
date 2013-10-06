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
#include "monome.h"
#include "screen.h"
// lppr
#include "app_timers.h"
#include "grid.h"
#include "handler.h"
#include "ctl.h"
#include "render.h"

// keep track of the last touched UI element (when appropriate)
/// fixme: might want to add app-specific events?
static eEventType touched = kNumSysEvents;

///////
////////
// FIXME
// timer is running at the wrong speed, so have to multiply for ms... gr
//static inline u32 ticks_ms(u32 
/////
///


//---------------------------------------------
//--------- static funcs

//--------------------------
//--- static func def

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


/* static s32 scale_knob_value_fast(s32 val) { */
/*   return scale_knob_value(val); */
/* } */

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
      ctl_tap_delay(0, ev->eventData);
    }
    render_sw_on(0, ev->eventData > 0);
    break;

  case kEventSwitch1:
    // tap delaytime 2
    if(ev->eventData > 0) {
      if(touchedThis) {
	render_touched_delaytime(1);
      }	
      ctl_tap_delay(1, ev->eventData);
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
    print_dbg("wtf footswitch 1");
    if(ev->eventData > 0) {
      ctl_loop_record(1);
    } 
    render_sw_on(2, ev->eventData > 0);
    break;
    
  case kEventSwitch7:
    print_dbg("wtf footswitch 2");
    if(ev->eventData > 0) {
      ctl_loop_playback(1);
    }
    render_sw_on(3, ev->eventData > 0);
    break;
    
  case kEventEncoder0:
    if(touchedThis) {
      render_touched_fb(0);
      //      kill_test();
      
    }
    ctl_inc_fb(0, scale_knob_value(ev->eventData));
    break;

  case kEventEncoder1:
    if(touchedThis) {
      render_touched_mix(0);

      //      kill_test();
    }
    ctl_inc_mix(0, scale_knob_value(ev->eventData));
    break;

  case kEventEncoder2:
    if(touchedThis) {
      render_touched_freq(0);

      //      kill_test();
    }
    ctl_inc_freq(0, scale_knob_value(ev->eventData));
    break;

  case kEventEncoder3:
    if(touchedThis) {
      render_touched_res(0);

      //      kill_test();
    }
    ctl_inc_res(0, scale_knob_value(ev->eventData));
    break;

    //-------- grid
  case kEventMonomeConnect :
    handle_monome_connect((u32)ev->eventData);
    break;

   case kEventMonomeGridKey:
     grid_handle_key_event(ev->eventData);
     break;


  default:
    break;
  }
}
