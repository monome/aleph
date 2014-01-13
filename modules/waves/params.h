
#ifndef _ALEPH_MODULE_WAVES_PARAMS_H_
#define _ALEPH_MODULE_WAVES_PARAMS_H_

//---------- defines
// ranges and radix
// ranges are in 16.16
// radix should be minimal bits to accommodate entire integer range.

//// these are defined in the oscillator dsp unit.
/* #define OSC_FREQ_MIN 0x040000      // 4 hz */
/* #define OSC_FREQ_MAX 0x40000000    // 16384 hz */
/* #define OSC_FREQ_RADIX 15 */

#define PARAM_HZ_MIN OSC_FREQ_MIN
#define PARAM_HZ_MAX OSC_FREQ_MIN
#define PARAM_HZ_DEFAULT (OSC_FREQ_MIN * 16)

#define PARAM_DAC_MIN 0
//#define PARAM_DAC_MAX (10 << 16)
// bah?
#define PARAM_DAC_MAX 0x7fffffff
#define PARAM_DAC_RADIX 16

#define RATIO_MIN 0x4000     // 1/4
#define RATIO_MAX 0x40000    // 4
#define RATIO_RADIX 3

#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 64
#define SMOOTH_FREQ_RADIX 7

// svf cutoff
#define PARAM_CUT_MAX     0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC

// rq
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x0000ffff
#define PARAM_RQ_DEFAULT 0x0000FFF0

#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

/// smoother default
// about 1ms?
#define PARAM_SMOOTH_DEFAULT  0x76000000

#define NUM_PARAMS eParamNumParams


// parameters
enum params {

  eParamFreq1,
  eParamFreq2,

  eParamTune1,
  eParamTune2,

  eParamWave1,
  eParamWave2,

  eParamAmp1,
  eParamAmp2,

  eParamPm12,
  eParamPm21,

  eParamWm12,
  eParamWm21,

  eParamBl1,
  eParamBl2,

  // filter 1
  eParam_cut1,		
  eParam_rq1,		
  eParam_low1,		
  eParam_high1,		
  eParam_band1,		
  eParam_notch1,	
  eParam_fwet1,		
  eParam_fdry1,		

  // filter 2
  eParam_cut2,
  eParam_rq2,
  eParam_low2,		
  eParam_high2,		
  eParam_band2,		
  eParam_notch2,	
  eParam_fwet2,		
  eParam_fdry2,		

  eParamFreq1Smooth,
  eParamFreq2Smooth,

  eParamPm12Smooth,
  eParamPm21Smooth,

  eParamWm12Smooth,
  eParamWm21Smooth,

  eParamWave1Smooth,
  eParamWave2Smooth,
  eParamAmp1Smooth,
  eParamAmp2Smooth,

  eParamIoAmp0,
  eParamIoAmp1, 
  eParamIoAmp2,
  eParamIoAmp3,

  eParam_dacVal0,
  eParam_dacVal1,
  eParam_dacVal2,
  eParam_dacVal3,
  eParam_dacSlew0,
  eParam_dacSlew1,
  eParam_dacSlew2,
  eParam_dacSlew3,


  eParamNumParams
};


extern void fill_param_desc(void);

#endif // h guard
