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
static const ampParamId[] = { eParam_adc0, 
			    eParam_adc1, 
			    eParam_adc2, 
			    eParam_adc3 };

//---------------------------------
//---- static function declarations

// set amplitude for a channel
static void ctl_set_amp(u32 ch, s32 val);
// set mute toggle for a channel
static void ctl_set_mute(u32 ch, s32 val);
// apply scaling to linear level control for one channel
static void ctl_scale_level(u32 ch);


//-------------------------
//---- extern function definitions

// get amplitude for a channel
s32 ctl_get_amp_db(u32 ch) {
  return ampDb[ch];
}

// get mute flag for a channel
s32 ctl_get_mute(u32 ch) {
  return mute[ch]; 
}

// toggle mute flag for a channel
void ctl_toggle_mute(u32 ch) {
  ch &= 3;
  ctl_set_mute(ch, mute[ch]^1);
}


//-----------------------------------
//--- static function definitions


// SET amplitude for a channel
static void ctl_set_amp(u32 ch, s32 val) {
  if(mute[ch]) {
    ;; // already muted, do nothing
  } else {
    ampVal[ch] = val;
    // send the linear amplitude as a param change to the DSP
    ctl_param_change(ampParamId[ch], val);
  }
  // redraw
  render_chan(ch); 
}

// set mute flag for a channel
static void ctl_set_mute(u32 ch, s32 val) {
  if(val > 0) {
    mute[ch] = 1;
    // send zero to the DSP
    ctl_param_change(ampParamId[ch], 0);
  } else {
    mute[ch] = 0;
    // send the linear amplitude as a param change to the DSP
    ctl_param_change(ampParamId[ch], ampLin[ch]);
  }
  // redraw
  render_chan(ch);
}

// apply scaling to linear level control for one channel
static void ctl_scale_level(u32 ch) {
  ampDb[ch] = 0;;
  ampDb[ch] = 0;;
}
