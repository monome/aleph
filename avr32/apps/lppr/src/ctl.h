
#ifndef _ALEPH_APP_LPPR_CTL_H_
#define _ALEPH_APP_LPPR_CTL_H_


#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x00400000
#define PARAM_BUFFER_MAX 0x002bf1ff // LINES_BUF_FRAMES - 1

//-- knob/cv -settable parameter values
// inputs. use s32 type but unsigned range (accumulators)
extern s32 in_fb[2];
extern s32 in_freq[2];
extern s32 in_res[2];
extern s32 in_mix[2];
extern s32 in_panInLR[2];
extern s32 in_panInFB[2];
extern s32 in_panOutLR[2];
extern s32 in_panOutFB[2];

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
extern u8 ctl_report_params(void);
// set parameters to initial values
extern void ctl_init_params(void);
// set delay time in ms
extern void ctl_set_delay_ms(u8 idx, u32 ms);
// tap delay time
extern void ctl_tap_delay(u8 idx, u8 val);
// set delay time multiplier
extern void ctl_set_delay_mul(u8 idx, u8 val);
// move both read and write heads by an offset multiple of tap time
// offset can be negative.. 
extern void ctl_set_delay_pos(u8 idx, u8 mul);
// set loop time as multiple of tap time
extern void ctl_set_loop(u8 idx, u8 mul);
// set pre level
extern void ctl_set_pre(u8 idx, fract32 val);

// start recording loop on given delayline
extern void ctl_loop_record(u8 idx);
// stop recording loop / start playback on given delayline
extern void ctl_loop_playback(u8 idx);

// set params from ui
extern void ctl_inc_fb(u8 id, s32 delta);
extern void ctl_inc_mix(u8 id, s32 delta);
extern void ctl_inc_freq(u8 id, s32 delta);
extern void ctl_inc_res(u8 id, s32 delta);

/// FIXME: uh, maybe just use the in / out params directly
/// instead of some weird quad-panning framework
extern void ctl_inc_panInLR(u8 id, s32 delta);
extern void ctl_inc_panInFB(u8 id, s32 delta);
extern void ctl_inc_panOutLR(u8 id, s32 delta);
extern void ctl_inc_panOutFB(u8 id, s32 delta);


// huge table of actual DSP parameters.
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
