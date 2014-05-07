
#ifndef _ALEPH_MODULE_MONO_PARAMS_H_
#define _ALEPH_MODULE_MONO_PARAMS_H_

#include "param_common.h"

//#define HZ_MIN 0x200000   // 32
//#define HZ_MAX 0x20000000 // 8192
//#define RATIO_MIN 0x2000 // 1/8
//#define RATIO_MAX 0x80000 // 8

#define OSC_FREQ_MIN 0x040000      // 4 hz */
#define OSC_FREQ_MAX 0x40000000    // 16384 hz */
#define OSC_FREQ_RADIX 15           //default 15

#define RATIO_MIN 0x2000     // 1/8
#define RATIO_MAX 0x80000    // 8
#define RATIO_RADIX 4

//#define ENV_DUR_MIN 0x0040   // 1/1024
//#define ENV_DUR_MAX 0x100000 // 32
//#define ENV_DUR_RADIX 5

#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 64
#define SMOOTH_FREQ_RADIX 7     //default 7


enum params {
    eParamFreq0,
    eParamFreq1,
    eParamFreq2,
    eParamFreq3,
    
    eParamWave0,
    eParamWave1,
    eParamWave2,
    eParamWave3,
    
    eParamAmp0,
    eParamAmp1,
    eParamAmp2,
    eParamAmp3,
    
    eParamIoAmp0,
    eParamIoAmp1, 
    eParamIoAmp2,
    eParamIoAmp3,
    
    eParamFreq0Slew,
    eParamFreq1Slew,
    eParamFreq2Slew,
    eParamFreq3Slew,
    
    eParamWave0Slew,
    eParamWave1Slew,
    eParamWave2Slew,
    eParamWave3Slew,
    
    eParamAmp0Slew,
    eParamAmp1Slew,
    eParamAmp2Slew,
    eParamAmp3Slew,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc* desc);

#endif
