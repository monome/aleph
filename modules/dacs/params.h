#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 64
#define SMOOTH_FREQ_RADIX 7

#define DAC_MIN 0x0000
#define DAC_MAX FRACT32_MAX
#define DAC_RADIX 32


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
