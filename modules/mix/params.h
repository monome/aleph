#ifndef _ALEPH_MIX_PARAMS_H_
#define _ALEPH_MIX_PARAMS_H_

#include "param_common.h"

// define some constants here for the param descriptor code
// here, all the parameter ranges are pretty simple.
#define PARAM_AMP_MAX 	0x7fffffff
#define bPARAM_CV_MAX 	0x7fffffff
#define PARAM_SLEW_MAX 	0x7fffffff

// something pretty fast, but noticeable
#define PARAM_SLEW_DEFAULT 	0x7ffecccc

// enumerate parameters
// the order defined here must be matched in the descriptor !
enum params {
  // cv slew
  eParam_cvSlew0,
  eParam_cvSlew1,
  eParam_cvSlew2,
  eParam_cvSlew3,
  // adc multiplier slew
  eparam_adcSlew0,
  eparam_adcSlew1,
  eparam_adcSlew2,
  eparam_adcSlew3,
  // cv values
  eParam_cv0,
  eParam_cv1,
  eParam_cv2,
  eParam_cv3,
  // adc multiplier values
  eparam_adc0,
  eparam_adc1,
  eparam_adc2,
  eparam_adc3,

  eParamNumParams
};

// this is only defined by the descriptor helper program.
extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
