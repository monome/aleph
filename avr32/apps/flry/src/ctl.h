
#ifndef _ALEPH_APP_FLRY_CTL_H_
#define _ALEPH_APP_FLRY_CTL_H_

#define PARAM_DAC_MIN 0x00000000
#define PARAM_DAC_MAX 0xffff


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

//---------------------------
//---- -external functions

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
u8 ctl_report_params(void);
// set parameters to initial values
void ctl_init_params(void);

// function key: send and change edit state
void ctl_key(u8 i, u8 val);
// button input: invert and send
void ctl_but(u8 i, u8 val);
// joystick axis: change value and send
void ctl_joy(u8 ch, u8 val);

// set dac value
void ctl_set_value(u8 channel, u16 val);
// increment value
void ctl_inc_value(u8 channel, s32 val);

// set slew
void ctl_set_slew(u8 channel, u16 value);
// increment slew
void ctl_inc_slew(u8 channel, s32 val);


#endif // h guard
