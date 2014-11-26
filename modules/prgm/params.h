#ifndef _ALEPH_MODULE_PRGM_PARAMS_H_
#define _ALEPH_MODULE_PRGM_PARAMS_H_

#include "param_common.h"

#define FIX_RADIX 16
#define BOOL_RADIX 2

enum params {
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
    
    eParamL0,
    eParamL1,
    eParamL2,
    eParamL3,
    
    eParamP0,
    eParamP1,
    eParamP2,
    eParamP3,
    
    eParamF0,
    eParamF1,
    eParamF2,
    eParamF3,
    
    eParamX0,
    eParamX1,
    eParamX2,
    eParamX3,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc *desc);

#endif
