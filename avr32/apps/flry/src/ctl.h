
#ifndef _ALEPH_APP_FLRY_CTL_H_
#define _ALEPH_APP_FLRY_CTL_H_

#define PARAM_DAC_MIN 0x00000000
#define PARAM_DAC_MAX 0xffff

// parameter enum
// FIXME: just copying from .ldr source for now
typedef enum params {
  // dac values
  eParam_dac0,
  eParam_dac1,
  eParam_dac2,
  eParam_dac3,
} eParam; 

//---------------------------
//---- -external functions

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
u8 ctl_report_params(void);
// set parameters to initial values
void ctl_init_params(void);
// set dac value
void  ctl_set_dac(u8 channel, u16 value);
// increment
void  ctl_inc_dac(u8 channel, s32 val);

#endif // h guard
