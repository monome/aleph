/*
  ctl.c

  aleph-mix

  this defines the control logic interface.

  our 'mix' app doesn't have to do much:
  
  - keep track of amplitude and mute values for each channel
  - re-render screen regions 
  - send parameter changes to blackfin
  
*/

// standard lib headers
#include <string.h>

//--- asf headers

// for delay_ms()
#include "delay.h"
// for print_dbg()
#include "print_funcs.h"

//--- -aleph-avr32 headers
// includes pause/resume
#include "app.h"
// control change functions
#include "control.h"

//---- aleph/common headers
// fixed-point representation
#include "fix.h"
// parameter types
#include "param_common.h"

//--- custom app headers
#include "app_timers.h"
#include "ctl.h"
#include "render.h"
#include "scaler.h"

//#include "util.h"

//-----------------
//---- static variables

// mute flags
static bool mute[4];

// linear control values for channel levels 
static s32 level[4];

// amplitude value after scaling, linear
// format is fract32 (blackfin native):
// [0x80000000, 0xffffffff] == [-1, 1)
static s32 ampLin[4];

// amplitude value after scaling, in decibels
// format is 16.16:
// 16b signed integer + 16b unsigned fract
static s32 ampDb[4];

// adc parameter indices
// keeping them here is clean and maintainable,
// at the cost of a little code space.
static const int ampParamId[] = { eParam_adc0, 
			    eParam_adc1, 
			    eParam_adc2,
			    eParam_adc3 };
 
//---------------------------------
//---- static function declarations

// set amplitude for a channel
static void ctl_set_amp(u32 ch);

// set mute for a channel
static void ctl_set_mute(u32 ch, bool val);

//-------------------------
//---- extern function definitions

// set the initial state
void ctl_init(void) {
  int i;
  // set inputs to defaults
  for(i=0; i<4; i++) {
    level[i] = maxLevelInput;
    scale_level(maxLevelInput, &ampLin[i], &ampDb[i]);
    ctl_set_amp(i);
  }
  // set other parameters to fixed values
  // adc multiplier slew
  // this integrator value is very fast, but enough to prevent clicks
  ctl_param_change(  eParam_adcSlew0, 0x7fe00000);
  ctl_param_change(  eParam_adcSlew1, 0x7fe00000);
  ctl_param_change(  eParam_adcSlew2, 0x7fe00000);
  ctl_param_change(  eParam_adcSlew3, 0x7fe00000);
  // cv slew
  ctl_param_change(  eParam_cvSlew0, 0);
  ctl_param_change(  eParam_cvSlew1, 0);
  ctl_param_change(  eParam_cvSlew2, 0);
  ctl_param_change(  eParam_cvSlew3, 0);
  // cv values
  ctl_param_change(  eParam_cv0, 0);
  ctl_param_change(  eParam_cv1, 0);
  ctl_param_change(  eParam_cv2, 0);
  ctl_param_change(  eParam_cv3, 0);

}

// get amplitude for a channel 
// (e.g. for rendering)
s32 ctl_get_amp_db(u32 ch) {
  return ampDb[ch];
}

// get mute flag for a channel 
// (e.g. for rendering)
s32 ctl_get_mute(u32 ch) {
  return mute[ch]; 
}

// toggle mute flag for a channel 
// (e.g. from switch handler)
void ctl_toggle_mute(u32 ch) {
  ch &= 3;
  ctl_set_mute(ch, mute[ch]^1);
}

// increment a level control 
// (e.g. from encoder handler)
extern void ctl_inc_level(u32 ch, s32 inc) {
  s32 l;
  ch &= 3;
  // clamp increment to prevent overflow
  if(inc < -65000) { inc = -65000; print_dbg("\r\n !! clamp inc low");  }
  if(inc > 65000) { inc = 65000; print_dbg("\r\n !! clamp inc high"); }
  l = level[ch] + inc;
  // clamp addition result 
  if(l < minLevelInput) { l = minLevelInput; }
  if(l > maxLevelInput) { l = maxLevelInput; }
  scale_level(l, &ampLin[ch], &ampDb[ch]);
  ctl_set_amp(ch);
  level[ch] = l;
  print_dbg("\r\n changed level control: 0x");
  print_dbg_hex(l);
}

//-----------------------------------
//--- static function definitions


// SET amplitude for a channel
static void ctl_set_amp(u32 ch) {
  if(mute[ch]) {
    ;; // already muted, do nothing
  } else {
    // send the linear amplitude as a param change to the DSP
    ctl_param_change(ampParamId[ch], ampLin[ch]);
  }
  // update graphics
  render_chan(ch); 
}

// set mute flag for a channel
static void ctl_set_mute(u32 ch, bool val) {
  if(val) {
    mute[ch] = 1;
    // send zero to the DSP
    ctl_param_change(ampParamId[ch], 0);
  } else {
    mute[ch] = 0;
    // send the linear amplitude as a param change to the DSP
    ctl_param_change(ampParamId[ch], ampLin[ch]);
  }
  // update graphics
  render_chan(ch);
}
