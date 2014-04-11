#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#include "param_common.h"

#define OSC_FREQ_MIN (4 << 16)
#define OSC_FREQ_MAX (8192 << 16)
#define OSC_FREQ_RADIX 8

#define PARAM_DAC_MIN 0
//#define PARAM_DAC_MAX (10 << 16)
// bah
#define PARAM_DAC_MAX 0x7fffffff
#define PARAM_DAC_RADIX 16


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

  eParam_auxL1,
  eParam_auxR1,
  eParam_pan1,
  eParam_fader1,

  eParam_auxL2,
  eParam_auxR2,
  eParam_pan2,
  eParam_fader2,

  eParam_auxL3,
  eParam_auxR3,
  eParam_pan3,
  eParam_fader3,

  eParamNumParams
};

extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
