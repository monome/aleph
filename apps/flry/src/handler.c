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
#include "grid.h"
#include "monome.h"
#include "app_timers.h"
#include "life.h"

// count of buttons
#define NUM_BUT 4
// count of joystick axes
#define NUM_JOY 4

// count of triggers
#define NUM_TRIG 4

//-----------------------------------------
//---- static vars

// button values
u8 but[NUM_BUT] = { 0, 0, 0, 0 };

// joystick values (2-axis)
// don't actually need to store/compare this with current system
// (each axis gets its own byte and the event means it changed)
volatile u8 joyVal[NUM_JOY] = {0, 0, 0, 0};

//---------------------------------------------
//--------- static funcs

// linear scaling for encoders
static s32 scale_lin_enc(s32 val) {
  return val << 7;    // *128
}

// exponential scaling for encoders
static s32 scale_knob_value(s32 val) {
  static const u32 kNumKnobScales_1 = 23;
  static const u32 knobScale[25] = {
    /// FIXME: should these be aligned better?
    /// would like to give best possible chance of falling on non-interpolated values.
    0x00000000,
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

  print_dbg("\r\n knob acc in: ");
  print_dbg_ulong(val);

  if(vabs > kNumKnobScales_1) {
    vabs = kNumKnobScales_1;
  }
  ret = knobScale[vabs];
  if(val < 0) {
    ret = BIT_NEG_ABS(ret);
  }
  return ret;
}

//--------------------------------
//----- handlers
static void handle_Switch0(s32 data) {
  life_change(1,1);
}
static void handle_Switch1(s32 data) {
  life_change(5,5);
}
static void handle_Switch2(s32 data) {
  if(data > 0) life_print();
}
static void handle_Switch3(s32 data) {
  life_init();
}
    
// decode a HID byte and make appropraite control changes
static void handle_HidByte(s32 data) {
  u8 idx, val;
  u8 b0, b1, b2, b3;
  
  // which 
  idx = (data & 0x0000ff00) >> 8;
  val = (data & 0x000000ff);

  /// FIXME: ok, just hardcoding this.
  /// fix to accomodate your brand of gamepad.

  /// TODO: a "learn" function is not so difficult,
  /// other option would be to maintain a database of vendor/protocols.

  switch(idx) {
  case 0:
    // joystick 1, x axis
    //// store these and poll later
    joyVal[0] = val;
    break;
  case 1:
    // joystick 1, y axis
    joyVal[1] = val;
    break;
  case 2:
    // joystick 2, x axis
    joyVal[2] = val;
    break;    
  case 3:
    // joystick 2, y axis
    joyVal[3] = val;
    break; 

  case 4:
    // on my joystick, the right-hand set of 4 buttons
    // are bitfields in upper nibble of this byte.
    b0 = (val & 0x10) > 0;
    b1 = (val & 0x20) > 0;
    b2 = (val & 0x40) > 0;
    b3 = (val & 0x80) > 0;

    if(b0 != but[0]) { but[0] = b0; ctl_but(0, b0); }
    if(b1 != but[1]) { but[1] = b1; ctl_but(1, b1); }
    if(b2 != but[2]) { but[2] = b2; ctl_but(2, b2); }
    if(b3 != but[3]) { but[3] = b3; ctl_but(3, b3); }
    // lower nibble is d-pad...
    break;
  }
}

static void handle_Encoder0(s32 data) {
  ctl_inc_value(3, scale_lin_enc(data));
}
static void handle_Encoder1(s32 data) {
  ctl_inc_value(2, scale_lin_enc(data));
}
static void handle_Encoder2(s32 data) {
  ctl_inc_value(1, scale_lin_enc(data));
}
static void handle_Encoder3(s32 data) {
  ctl_inc_value(0, scale_lin_enc(data));
}

static void handle_MonomeConnect(s32 data) {
  eMonomeDevice dev;
  u8 w;
  u8 h;
  monome_connect_parse_event_data((u32)data, &dev, &w, &h);
  if(dev != eDeviceGrid) {
    print_dbg("\r\nmonome connect: unsupported device");
    return;
  }
  print_dbg("\r\nconnecting grid device");
  // grid_set_size(w, h);
  timers_set_monome();
}

static void handle_MonomeDisconnect(s32 data) {
  timers_unset_monome();
}

static void handle_MonomeGridKey(s32 data) {
  grid_handle_key_event(data);
}


//---------------------------------------
//---- external funcs

void flry_assign_event_handlers(void) {
  app_event_handlers[ kEventSwitch0 ] = &handle_Switch0;
  app_event_handlers[ kEventSwitch1 ] = &handle_Switch1;
  app_event_handlers[ kEventSwitch2 ] = &handle_Switch2;
  app_event_handlers[ kEventSwitch3 ] = &handle_Switch3;

  app_event_handlers[ kEventEncoder0 ] = &handle_Encoder0;
  app_event_handlers[ kEventEncoder1 ] = &handle_Encoder1;
  app_event_handlers[ kEventEncoder2 ] = &handle_Encoder2;
  app_event_handlers[ kEventEncoder3 ] = &handle_Encoder3;

  app_event_handlers[ kEventMonomeConnect ] = &handle_MonomeConnect;
  app_event_handlers[ kEventMonomeDisconnect ] = &handle_MonomeDisconnect;
  app_event_handlers[ kEventMonomeGridKey ] = &handle_MonomeGridKey;

}
