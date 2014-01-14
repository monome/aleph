#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

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
  eParamNumParams
};  

extern void fill_param_desc(void);

#endif // header guard 
// EOF
