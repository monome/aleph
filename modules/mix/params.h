#ifndef _ALEPH_MIX_PARAMS_H_
#define _ALEPH_MIX_PARAMS_H_

#include "param_common.h"

// define some constants here for the param descriptor code
// here, all the parameter ranges are pretty simple.
#define PARAM_AMP_MAX 	0x7fffffff
#define PARAM_CV_MAX 	0x7fffffff
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

// this is only defined by the descriptor helper program.
extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
