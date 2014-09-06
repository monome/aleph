#ifndef _ALEPH_MODULE_PRGM_PARAMS_H_
#define _ALEPH_MODULE_PRGM_PARAMS_H_

#include "param_common.h"

#define OSC_HZ_MIN 0x00010000           //1 hz
#define OSC_HZ_MAX 0x40000000           //16384 hz
#define OSC_HZ_RADIX 15

//017 TEST
//#define CV_VOCT_MAX 0x78000000        //10 octaves at 1v/octave
#define CV_VOCT_MAX FR32_MAX            //10 octaves at 1v/octave
#define CV_VOCT_RADIX 16

#define PARAM_DAC_MIN 0
#define PARAM_DAC_MAX 0x7fffffff
#define PARAM_DAC_RADIX 16
#define PARAM_DAC_SHIFT (PARAM_DAC_RADIX) - 1

#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)
#define PARAM_SLEW_DEFAULT  0x76000000

enum params {
    eParamFreq0,
    eParamFreq1,
    eParamFreq2,
    eParamFreq3,
    
    eParamTranspose0,
    eParamTranspose1,
    eParamTranspose2,
    eParamTranspose3,

    eParamTab0,
    eParamTab1,
    eParamTab2,
    eParamTab3,
    
    eParamWave0,
    eParamWave1,
    eParamWave2,
    eParamWave3,
    
    eParamSyncTrig,
    
    eParamTripPoint0,
    eParamTripPoint1,
    eParamTripPoint2,
    eParamTripPoint3,
        
    eParamAmp0,
    eParamAmp1,
    eParamAmp2,
    eParamAmp3,
    
    eParamCVfreq0,
    eParamCVfreq1,
    eParamCVfreq2,
    eParamCVfreq3,
    
    eParamCVtranspose0,
    eParamCVtranspose1,
    eParamCVtranspose2,
    eParamCVtranspose3,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc *desc);

#endif
