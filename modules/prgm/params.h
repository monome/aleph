#ifndef _ALEPH_MODULE_PRGM_PARAMS_H_
#define _ALEPH_MODULE_PRGM_PARAMS_H_

#include "param_common.h"

#define FIX_RADIX 16

enum params {

    //  sequenced parameters
    //0
    eParamTrig0,
    eParamTrig1,
    eParamTrig2,
    eParamTrig3,

    eParamFlag0,
    eParamFlag1,
    eParamFlag2,
    eParamFlag3,
    
    eParamCurve0,
    eParamCurve1,
    eParamCurve2,
    eParamCurve3,
    
    eParamTime0,
    eParamTime1,
    eParamTime2,
    eParamTime3,
    
    //16
    eParamInput0,
    eParamInput1,
    eParamInput2,
    eParamInput3,
    
    eParamPosition0,
    eParamPosition1,
    eParamPosition2,
    eParamPosition3,
    
    eParamLoop0,
    eParamLoop1,
    eParamLoop2,
    eParamLoop3,
    
    eParamLevel0,
    eParamLevel1,
    eParamLevel2,
    eParamLevel3,
    
    //32
    eParamFrequency0,
    eParamFrequency1,
    eParamFrequency2,
    eParamFrequency3,
    
    eParamFrames,

    //  global parameters
    //eParamSqLength,
    
    eParamBufferLength0,
    eParamBufferLength1,
    eParamBufferLength2,
    eParamBufferLength3,
    
    eParamToggleBuf,
    eParamToggleRec,
    
    //43
    eParamSqCounter,
    eParamSqLength,

    eParamFrameFwd,
    eParamFrameBwd,
    
    eParamToggleMotor,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc *desc);

#endif
