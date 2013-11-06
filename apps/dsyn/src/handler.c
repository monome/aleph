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

//---------------------------------------------
//--------- static funcs

static s32 scale_knob_value(s32 val) {
  // exponential table
  static const u32 kNumKnobScales = 24;
  //  static const u32 kNumKnobScales_1 = 23;
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

  if(val == 0) { return ret; }

  if(vabs > kNumKnobScales) {
    vabs = kNumKnobScales;
  }
  ret = knobScale[vabs-1];

  if(val < 0) {
    ret = BIT_NEG_ABS(ret);
  }

  return ret;
}

static inline void handle_sw(u8 id, u8 b) {
  render_sw_on(id, b);
  if(b) {
    ctl_set_voice_select(id);
  }			   
}

//-----------------------
//--- handlers

///// this is an application-specific event.
/// use it so that sequence processing can take place
// outside the ISR / timer callback
static void handle_AppCustom(s32 data) {
  seq_advance();
}

static void handle_Switch0(s32 data) {
  handle_sw(0, data > 0);
}

static void handle_Switch1(s32 data) {
  handle_sw(1, data > 0);
}

static void handle_Switch2(s32 data) {
  handle_sw(2, data > 0);
}

static void handle_Switch3(s32 data) {
  handle_sw(3, data > 0);
}

static void handle_Switch4(s32 data) {
  // mode button: write config file
  if(data > 0) {
    print_dbg("\r\n write params...");
    files_write_params();
  }
}

static void handle_Switch6(s32 data) {
  render_sw_on(2, data > 0);
}

static void handle_Switch7(s32 data) {
  render_sw_on(3, data > 0);
}

static void handle_MonomeGridKey(s32 data) {
  grid_handle_key_event(data);
}

static void handle_Encoder0(s32 data) {
  /// TEMPO
  ctl_inc_tempo(scale_knob_value(data));
}

static void handle_Encoder1(s32 data) {
  // SCROLL GRID
  grid_inc_scroll( data > 0 ? 1 : -1);
}

static void handle_Encoder2(s32 data) {
  // PARAM VALUE
  ctl_inc_param( scale_knob_value(data) );
}

static void handle_Encoder3(s32 data) {
  // PARAM IDX 
  ctl_inc_param_select( data > 0 ? 1 : -1 );
}

static void handle_MonomeConnect (s32 data) {
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
static void handle_MonomeDisconnect (s32 data) {
  timers_unset_monome();
}

//---------------------------------------
//---- external funcs
void dsyn_assign_event_handlers(void) {
  /// app-specific:
  app_event_handlers[ kEventAppCustom ]	= &handle_AppCustom ;
  // system-defined:
  app_event_handlers[ kEventEncoder0 ]	= &handle_Encoder0 ;
  app_event_handlers[ kEventEncoder1 ]	= &handle_Encoder1 ;
  app_event_handlers[ kEventEncoder2 ]	= &handle_Encoder2 ;
  app_event_handlers[ kEventEncoder3 ]	= &handle_Encoder3 ;
  //// FIXME: use mode / power keys
  app_event_handlers[ kEventSwitch0 ]	= &handle_Switch0 ;
  app_event_handlers[ kEventSwitch1 ]	= &handle_Switch1 ;
  app_event_handlers[ kEventSwitch2 ]	= &handle_Switch2 ;
  app_event_handlers[ kEventSwitch3 ]	= &handle_Switch3 ;
  //// FIXME: use power key for soft-powerdown and save.
  /// for now, mode key stores configuration
  app_event_handlers[ kEventSwitch4 ]	= &handle_Switch4 ;
  //  app_event_handlers[ kEventSwitch5 ]	= &handle_Switch5 ;
  app_event_handlers[ kEventSwitch6 ]	= &handle_Switch6 ;
  app_event_handlers[ kEventSwitch7 ]	= &handle_Switch7 ;
  app_event_handlers[ kEventMonomeConnect ]	= &handle_MonomeConnect ;
  app_event_handlers[ kEventMonomeDisconnect ]	= &handle_MonomeDisconnect ;
  app_event_handlers[ kEventMonomeGridKey ]	= &handle_MonomeGridKey ;
}
