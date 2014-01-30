
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
extern s32 in_fwet[2];
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
// set read head position of loop, as subdivision of loop time
void ctl_set_loop_pos(u8 idx, u8 mul);

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

// enumerate parameters
typedef enum params {

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
  eParam_fwet0,			// 24
  eParam_fdry0,			// 25

  // filter 1
  eParam_coeff1,		// 26
  eParam_rq1,			// 27
  eParam_low1,			// 28
  eParam_high1,			// 29
  eParam_band1,			// 30
  eParam_notch1,		// 31
  eParam_fwet1,			// 32
  eParam_fdry1,			// 33

  // delay input mix
  eParam_adc0_del0,		// 34
  eParam_adc0_del1,		// 35
  eParam_adc1_del0,		// 36
  eParam_adc1_del1,		// 37
  eParam_adc2_del0,		// 38
  eParam_adc2_del1,		// 39
  eParam_adc3_del0,		// 40
  eParam_adc3_del1,		// 41

  // feedback mix
  eParam_del0_del0,		// 42
  eParam_del0_del1,		// 43
  eParam_del1_del0,		// 44
  eParam_del1_del1,		// 45

  // delay output mix
  eParam_del0_dac0,		// 46
  eParam_del0_dac1,		// 47
  eParam_del0_dac2,		// 48
  eParam_del0_dac3,		// 49
  eParam_del1_dac0,		// 
  eParam_del1_dac1,		// 50
  eParam_del1_dac2,		// 51
  eParam_del1_dac3,		// 52

  // dry mix
  eParam_adc0_dac0,		// 53
  eParam_adc0_dac1,		// 54
  eParam_adc0_dac2,		// 55
  eParam_adc0_dac3,		// 56
  eParam_adc1_dac0,		// 57
  eParam_adc1_dac1,		// 58
  eParam_adc1_dac2,		// 59
  eParam_adc1_dac3,		// 60
  eParam_adc2_dac0,		// 61
  eParam_adc2_dac1,		// 62
  eParam_adc2_dac2,		// 63
  eParam_adc2_dac3,		// 64
  eParam_adc3_dac0,		// 65
  eParam_adc3_dac1,		// 66
  eParam_adc3_dac2,		// 67
  eParam_adc3_dac3,		// 68

  eParamNumParams
} eParam;  

//---------------------------
//---- -external functions

#endif // h guard
