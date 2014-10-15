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
// control change functions. 
// (as of now, just a thin wrapper around blackfin spi comm layer.)
#include "control.h"

//---- aleph/common headers
// fixed-point representation
#include "fix.h"
// parameter types
#include "param_common.h"

//--- custom app headers
#include "app_timers.h"
#include "ctl.h"
//#include "files.h"
//#include "inputs.h"
#include "render.h"

//#include "util.h"
//--- these are actually just copied from BEES
// #include "scalers/scaler_amp.h"
// #include "scalers/scaler_integrator.h"

//-----------------
//---- static variables

// raw values for adc multiplier parameters
static s32 adc[4];
// mute flags
static bool mute[4];
// current amp values
static bool ampVal[4] = { 0, 0, 0, 0 };

//-------------------------
//---- extern function definitions

// set amplitude for a channel
void ctl_set_amp(u32 ch, s32 val) {
  // limit
  ch &= 3;
  // check the mute flag
  if(mute[ch]) {
    ;; 
  } else {
    // a dumb hack for param idx using first adc param as offset
    ampVal[ch] = val;
    ctl_param_change(eParam_adc0 + ch, val);
  }

  // let's just echo it to the cv output, regardless of mute status
  ctl_param_change(eParam_cv0 + ch, val);

  // draw the amplitude change
  render_amp(ch); 
}

// set mute flag for a channel
void ctl_set_mute(u32 ch, s32 val) {
  ch &= 3;
  // set the flag
  if(val > 0) {
    mute[ch] = 1;
    ctl_param_change(eParam_adc0 + ch, 0);
  } else {
    mute[ch] = 0;
    ctl_param_change(eParam_adc0 + ch, ampVal[ch]);
  }
  render_mute(ch);
}

// get amplitude for a channel
s32 ctl_get_amp(u32 ch) {
  return adc[ch & 3];
}

// get mute toggle for a channel
s32 ctl_get_mute(u32 ch) {
  return mute[ch & 3];
}
