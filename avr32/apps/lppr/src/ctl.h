#ifndef _ALEPH_APP_LPPR_CTL_H_
#define _ALEPH_APP_LPPR_CTL_H_

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
u8 report_params(void);
// set parameters to initial values
void set_initial_params(void);
// set delay time in ms
void  param_set_delay_ms(u8 idx, u32 ms);
// start recording loop on given delayline
void param_loop_record(u8 idx);
// stop recording loop / start playback on given delayline
void param_loop_playback(u8 idx);

#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x00400000

#define PARAM_BUFFER_MAX 0x002bf1ff // LINES_BUF_FRAMES - 1

#define PARAM_COEFF_MAX 0x1F400000  // fixme


// FIXME: this is blithely copied from aleph/bfin/modules/lines/param.h
// i suppose there should be some build mechanism to share sources directly.
typedef enum params {

  // delay 0
  eParam_delay0,
  eParam_loop0,
  eParam_rate0,
  eParam_write0,
  eParam_pre0,
  eParam_pos_write0,
  eParam_pos_read0,
  eParam_run_read0,
  eParam_run_write0,

  // delay 1
  eParam_delay1,
  eParam_loop1,
  eParam_rate1,
  eParam_write1,
  eParam_pre1,
  eParam_pos_write1,
  eParam_pos_read1,
  eParam_run_read1,
  eParam_run_write1,

  // filter 0
  eParam_coeff0,
  eParam_rq0,
  eParam_low0,
  eParam_high0,
  eParam_band0,
  eParam_notch0,
  eParam_mix0,

  // filter 1
  eParam_coeff1,
  eParam_rq1,
  eParam_low1,
  eParam_high1,
  eParam_band1,
  eParam_notch1,
  eParam_mix1,

  // delay input mix
  eParam_adc0_del0,
  eParam_adc0_del1,
  eParam_adc1_del0,
  eParam_adc1_del1,
  eParam_adc2_del0,
  eParam_adc2_del1,
  eParam_adc3_del0,
  eParam_adc3_del1,

  // feedback mix
  eParam_del0_del0,
  eParam_del0_del1,
  eParam_del1_del0,
  eParam_del1_del1,

  // delay output mix
  eParam_del0_dac0,
  eParam_del0_dac1,
  eParam_del0_dac2,
  eParam_del0_dac3,
  eParam_del1_dac0,
  eParam_del1_dac1,
  eParam_del1_dac2,
  eParam_del1_dac3,

  // dry mix
  eParam_adc0_dac0,
  eParam_adc0_dac1,
  eParam_adc0_dac2,
  eParam_adc0_dac3,
  eParam_adc1_dac0,
  eParam_adc1_dac1,
  eParam_adc1_dac2,
  eParam_adc1_dac3,
  eParam_adc2_dac0,
  eParam_adc2_dac1,
  eParam_adc2_dac2,
  eParam_adc2_dac3,
  eParam_adc3_dac0,
  eParam_adc3_dac1,
  eParam_adc3_dac2,
  eParam_adc3_dac3,
  eParamNumParams
} eParam; 

//---------------------------
//---- -external functions

#endif // h guard
