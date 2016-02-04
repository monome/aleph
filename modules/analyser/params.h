#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#include "param_common.h"

#define PARAM_DAC_MIN 0
#define PARAM_DAC_MAX FR32_MAX
#define PARAM_DAC_RADIX 16

#define PARAM_AMP_0 (FRACT32_MAX)
#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

#define PARAM_CV_SLEW_DEFAULT 0x77000000

// cv output
#define PARAM_CV_VAL_DEFAULT PARAM_AMP_6

// enumerate parameters
enum params {
  //these are the results of pitch/amplitude analysis
  eParam_linEnv,
  eParam_logEnv,
  eParam_pitch,

  //these are the envelope params for amplitude analysis
  eParam_linAttack,
  eParam_linDecay,

  eParam_logAttack,
  eParam_logDecay,

  //standard DC CV param on the last CV output
  eParam_cvVal3,
  eParam_cvSlew3,

  eParamNumParams
};

extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
