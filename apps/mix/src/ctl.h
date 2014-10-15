/*
  ctl.h

  aleph-mix

  this declares the control logic interface.

  our 'mix' app doesn't have to do much, just keep track of amplitude and mute values for each channel.
  
*/

#ifndef _ALEPH_APP_MIX_CTL_H_
#define _ALEPH_APP_MIX_CTL_H_

#include "types.h"
//---------------------------
//---- variables

// param enumeration baldly copied from blackfin module source.
// ( in this case, modules/mix/params.h )
// this really is just the easiest way to handle it.
enum params {
  // cv slew
  eParam_cvSlew0,
  eParam_cvSlew1,
  eParam_cvSlew2,
  eParam_cvSlew3,
  // adc multiplier slew
  eParam_adcSlew0,
  eParam_adcSlew1,
  eParam_adcSlew2,
  eParam_adcSlew3,
  // cv values
  eParam_cv0,
  eParam_cv1,
  eParam_cv2,
  eParam_cv3,
  // adc multiplier values
  eParam_adc0,
  eParam_adc1,
  eParam_adc2,
  eParam_adc3,

  eParamNumParams
};


//---------------------------
//---- -external functions

// set initial values
extern void ctl_init(void);

// increment a level control (e.g. by encoder)
extern void ctl_inc_level(u32 ch, s32 inc);

// toggle mute flag for a channel
void ctl_toggle_mute(u32 ch);

// get amplitude for a channel
extern s32 ctl_get_amp_db(u32 ch);

// get mute toggle for a channel
extern s32 ctl_get_mute(u32 ch);


#endif // h guard

