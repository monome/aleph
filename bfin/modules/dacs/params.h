#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

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
