#ifndef _ALEPH_LINES_PARAMS_H_
#define _ALEPH_LINES_PARAMS_H_

#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x00400000

#define PARAM_BUFFER_MAX 0x002bf1ff // LINES_BUF_FRAMES - 1

#define PARAM_COEFF_MAX 0x1F400000;  // fixme

// enumerate parameters
enum params {

  // delay 0
  eParam_delay0,		// 0
  eParam_loop0,			// 1 
  eParam_rate0,			// 2
  eParam_write0,		// 3
  eParam_pre0,			// 4
  eParam_pos_write0,		// 5
  eParam_pos_read0,		// 6
  eParam_run_read0,		// 7
  eParam_run_write0,		// 8

  // delay 1
  eParam_delay1,		// 9
  eParam_loop1,			// 10
  eParam_rate1,			// 11
  eParam_write1,		// 12
  eParam_pre1,			// 13
  eParam_pos_write1,		// 14
  eParam_pos_read1,		// 15
  eParam_run_read1,		// 16
  eParam_run_write1,		// 17

  // filter 01
  eParam_coeff0,		// 18
  eParam_rq0,			// 19
  eParam_low0,			// 20
  eParam_high0,			// 21
  eParam_band0,			// 22
  eParam_notch0,		// 23
  eParam_mix0,			// 24

  // filter 1
  eParam_coeff1,		// 25
  eParam_rq1,			// 26
  eParam_low1,			// 27
  eParam_high1,			// 28
  eParam_band1,			// 29
  eParam_notch1,		// 30
  eParam_mix1,			// 31

  // delay input mix
  eParam_adc0_del0,		// 32
  eParam_adc0_del1,		// 33
  eParam_adc1_del0,		// 34
  eParam_adc1_del1,		// 35
  eParam_adc2_del0,		// 36
  eParam_adc2_del1,		// 37
  eParam_adc3_del0,		// 38
  eParam_adc3_del1,		// 39

  // feedback mix
  eParam_del0_del0,		// 40
  eParam_del0_del1,		// 41
  eParam_del1_del0,		// 42
  eParam_del1_del1,		// 43

  // delay output mix
  eParam_del0_dac0,		// 44
  eParam_del0_dac1,		// 45
  eParam_del0_dac2,		// 46
  eParam_del0_dac3,		// 47
  eParam_del1_dac0,		// 48
  eParam_del1_dac1,		// 49
  eParam_del1_dac2,		// 50
  eParam_del1_dac3,		// 51

  // dry mix
  eParam_adc0_dac0,		// 52
  eParam_adc0_dac1,		// 53
  eParam_adc0_dac2,		// 54
  eParam_adc0_dac3,		// 55
  eParam_adc1_dac0,		// 56
  eParam_adc1_dac1,		// 57
  eParam_adc1_dac2,		// 58
  eParam_adc1_dac3,		// 59
  eParam_adc2_dac0,		// 60
  eParam_adc2_dac1,		// 61
  eParam_adc2_dac2,		// 62
  eParam_adc2_dac3,		// 63
  eParam_adc3_dac0,		// 64
  eParam_adc3_dac1,		// 65
  eParam_adc3_dac2,		// 66
  eParam_adc3_dac3,		// 67

  eParamNumParams
};  

extern void fill_param_desc(void);

#endif // header guard 
// EOF
