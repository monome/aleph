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


#define EFFECT_TYPE eParamTypeAmp
#define EFFECT_MIN 0x00000000
#define EFFECT_MAX FR32_MAX
#define EFFECT_RADIX 1

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

#define DELAY_TYPE eParamTypeFix
#define DELAY_MIN 0x00000000
#define DELAY_MAX FR32_MAX
#define DELAY_RADIX 8

#define PAN_TYPE eParamTypeFix
#define PAN_MIN 0x00000000
#define PAN_MAX FR32_MAX
#define PAN_RADIX 16

// enumerate parameters
enum params {
  eParam_dac0,
  eParam_dac1,
  eParam_dac2,
  eParam_dac3,
  eParam_slew0,
  eParam_slew1,
  eParam_slew2,
  eParam_slew3,

  eParam_auxL0,
  eParam_auxR0,
  eParam_pan0,
  eParam_fader0,
  eParam_effect0,

  eParam_auxL1,
  eParam_auxR1,
  eParam_pan1,
  eParam_fader1,
  eParam_effect1,

  eParam_auxL2,
  eParam_auxR2,
  eParam_pan2,
  eParam_fader2,
  eParam_effect2,

  eParam_auxL3,
  eParam_auxR3,
  eParam_pan3,
  eParam_fader3,
  eParam_effect3,

  eParam_delay0,
  eParam_delay0Slew,
  eParam_feedback0,

  eParamNumParams
};

extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
