
#ifndef _ALEPH_MODULE_WAVES_PARAMS_H_
#define _ALEPH_MODULE_WAVES_PARAMS_H_

#include "param_common.h"

//---------- defines
// ranges and radix
// ranges are in 16.16
// radix should be minimal bits to accommodate entire integer range.
// note that this includes an obligatory sign bit, even if min >= 0

#define PARAM_HZ_MIN OSC_FREQ_MIN
#define PARAM_HZ_MAX OSC_FREQ_MAX
#define PARAM_HZ_DEFAULT (OSC_FREQ_MIN * 16)

#define PARAM_DAC_MIN 0
#define PARAM_DAC_MAX 0x7fffffff
#define PARAM_DAC_RADIX 16
#define PARAM_DAC_SHIFT (PARAM_DAC_RADIX) - 1

#define RATIO_MIN 0x4000     // 1/4
#define RATIO_MAX 0x40000    // 4
#define RATIO_RADIX 3

#define SLEW_SECONDS_MIN 0x2000 // 1/8
#define SLEW_SECONDS_MAX 0x400000 // 64
#define SLEW_SECONDS_RADIX 7

// svf cutoff
#define PARAM_CUT_MAX     0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC

// rq
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x0000ffff
#define PARAM_RQ_DEFAULT 0x0000FFF0


// fm delay
#define PARAM_FM_DEL_MIN 0
#define PARAM_FM_DEL_MAX 0xffff
#define PARAM_FM_DEL_DEFAULT 0xff
#define PARAM_FM_DEL_RADIX 1

#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

/// smoother default
// about 1ms?
#define PARAM_SLEW_DEFAULT  0x76000000

// mode index in 16.16
#define PARAM_MODE_MIN 0
#define PARAM_MODE_MAX 0x30000
#define PARAM_MODE_RADIX 3


// base-frequency limits in fix16
#define OSC_HZ_MIN 0x00010000      // 1 hz
#define OSC_HZ_MAX 0x40000000    // 16384 hz
#define OSC_HZ_RADIX 15


#define NUM_PARAMS eParamNumParams

// parameters
enum params {

  /// smoothers have to be processed first!
  eParamAmp0Slew,
  eParamAmp1Slew,

  eParamHz0Slew,
  eParamHz1Slew,

  eParamWave0Slew,
  eParamWave1Slew,

  eParamPm10Slew,
  eParamPm01Slew,

  eParamWm10Slew,
  eParamWm01Slew,

  eParamCut0Slew,
  eParamCut1Slew,

  eParamRq0Slew,
  eParamRq1Slew,

  eParamDry0Slew,
  eParamDry1Slew,

  eParamWet0Slew,
  eParamWet1Slew,

  // i/o patch points 
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

  /// osc out mix
  eParam_osc0_dac0,
  eParam_osc0_dac1,
  eParam_osc0_dac2,
  eParam_osc0_dac3,

  eParam_osc1_dac0,
  eParam_osc1_dac1,
  eParam_osc1_dac2,
  eParam_osc1_dac3,
 
  // cv
  eParam_cvSlew3,
  eParam_cvSlew2,
  eParam_cvSlew1,
  eParam_cvSlew0,

  eParam_cvVal3,
  eParam_cvVal2,
  eParam_cvVal1,
  eParam_cvVal0,

  // filter 1
  eParam_cut1,		
  eParam_rq1,		
  eParam_mode1,
  eParam_fwet1,		
  eParam_fdry1,		

  // filter 0
  eParam_cut0,
  eParam_rq0,
  eParam_mode0,
  eParam_fwet0,		
  eParam_fdry0,		

  // osc parameters

  eParamFmDel0,
  eParamFmDel1,

  /* eParamBl1, */
  /* eParamBl0, */

  eParamWm10,
  eParamWm01,

  eParamPm10,
  eParamPm01,

  eParamWave1,
  eParamWave0,

  eParamAmp1,
  eParamAmp0,

  eParamTune1,
  eParamTune0,

  eParamHz1,
  eParamHz0,

  eParamNumParams
};


extern void fill_param_desc(ParamDesc* desc);

#endif // h guard
