#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#include "param_common.h"

#define OSC_FREQ_MIN (4 << 16)
#define OSC_FREQ_MAX (8192 << 16)
#define OSC_FREQ_RADIX 8

#define PARAM_DAC_MIN 0
//#define PARAM_DAC_MAX (10 << 16)
// bah
#define PARAM_DAC_MAX FR32_MAX
#define PARAM_DAC_RADIX 16

#define PARAM_AMP_0 (FRACT32_MAX)
#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

#define PARAM_SLEW_DEFAULT 0x77000000

// cv output
#define PARAM_CV_VAL_DEFAULT PARAM_AMP_6


#define FADER_TYPE eParamTypeAmp
#define FADER_MIN 0x00000000
#define FADER_MAX FR32_MAX
#define FADER_RADIX 1

/*
//In bees this looks like 0-62 (with decimal places)
//In module this comes out as 0x00000000 - 0x00007FFF
#define DELAY_TYPE eParamTypeIntegrator
#define DELAY_MIN 0x00000000
#define DELAY_MAX 0x7FFFFFFF
#define DELAY_RADIX 1
*/

#define PARAM_SECONDS_MAX 0x003c0000
#define PARAM_SECONDS_RADIX 7

#define TIME_MS_TYPE eParamTypeFix
#define TIME_MS_MIN 0x00000000
#define TIME_MS_MAX FR32_MAX
#define TIME_MS_RADIX 18

#define TIME_SAMPLE_TYPE eParamTypeFix
#define TIME_SAMPLE_MIN 0x00000000
#define TIME_SAMPLE_MAX FR32_MAX
#define TIME_SAMPLE_RADIX 16

#define TIME_SUBSAMPLE eParamTypeFix
#define TIME_SUBSAMPLE_MIN 0x00000000
#define TIME_SUBSAMPLE_MAX FR32_MAX
#define TIME_SUBSAMPLE_RADIX 8

#define SPEED_TYPE TIME_SUBSAMPLE
#define SPEED_MIN TIME_SUBSAMPLE_MIN
#define SPEED_MAX TIME_SUBSAMPLE_MAX
#define SPEED_RADIX TIME_SUBSAMPLE_RADIX

#define PAN_TYPE eParamTypeFix
#define PAN_MIN 0x00000000
#define PAN_MAX FR32_MAX
#define PAN_RADIX 16

// enumerate parameters
enum params {
  //adc mix params
  eParam_fader_i1,
  eParam_pan_i1,
  eParam_aux1_i1,
  eParam_aux2_i1,
  eParam_effect_i1,

  eParam_fader_i2,
  eParam_pan_i2,
  eParam_aux1_i2,
  eParam_aux2_i2,
  eParam_effect_i2,

  eParam_fader_i3,
  eParam_pan_i3,
  eParam_aux1_i3,
  eParam_aux2_i3,
  eParam_effect_i3,

  eParam_fader_i4,
  eParam_pan_i4,
  eParam_aux1_i4,
  eParam_aux2_i4,
  eParam_effect_i4,

  //grain mix params
  eParam_source_g1,
  eParam_fader_g1,
  eParam_pan_g1,
  eParam_aux1_g1,
  eParam_aux2_g1,
  eParam_effect_g1,
  eParam_phase_g1, //So we can flip the phase back into effect bus for flanging fb

  //grain scrubber params
  eParam_scrubPitch_g1,
  eParam_scrubLength_g1,
  eParam_scrubFadeLength_g1,

  //grain echo params
  eParam_echoTime_g1,
  eParam_echoSpeed_g1,
  eParam_echoEdgeBehaviour_g1,
  eParam_echoFadeLength_g1,
  eParam_echoMin_g1,
  eParam_echoMax_g1,

  //grain writehead params
  eParam_slewSpeed_g1,
  eParam_writeEnable_g1,

  //grain mix params
  eParam_source_g2,
  eParam_fader_g2,
  eParam_pan_g2,
  eParam_aux1_g2,
  eParam_aux2_g2,
  eParam_effect_g2,
  eParam_phase_g2, //So we can flip the phase back into effect bus for flanging fb

  //grain scrubber params
  eParam_scrubPitch_g2,
  eParam_scrubLength_g2,
  eParam_scrubFadeLength_g2,

  //grain echo params
  eParam_echoTime_g2,
  eParam_echoSpeed_g2,
  eParam_echoEdgeBehaviour_g2,
  eParam_echoFadeLength_g2,
  eParam_echoMin_g2,
  eParam_echoMax_g2,

  //grain writehead params
  eParam_slewSpeed_g2,
  eParam_writeEnable_g2,
  
  eParamNumParams
};

extern void fill_param_desc(ParamDesc* desc);

void param_desc_aux (ParamDesc* desc,int param_idx,
		     char* desc_string, ParamType type,
		     s32 min, s32 max, u8 radix);

#endif // header guard 
// EOF
