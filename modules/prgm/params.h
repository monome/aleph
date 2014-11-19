#ifndef _ALEPH_MODULE_PRGM_PARAMS_H_
#define _ALEPH_MODULE_PRGM_PARAMS_H_

#include "param_common.h"

#define FIX_RADIX 16
#define BOOL_RADIX 2

enum params {
    eParamFree0,
    eParamFree1,
    eParamFree2,
    eParamFree3,
    
    eParamTransposed0,
    eParamTransposed1,
    eParamTransposed2,
    eParamTransposed3,
    
    eParamCurveTime0,
    eParamCurveTime1,
    eParamCurveTime2,
    eParamCurveTime3,
    
    eParamCurve0,
    eParamCurve1,
    eParamCurve2,
    eParamCurve3,

    eParamCurveDest0,
    eParamCurveDest1,
    eParamCurveDest2,
    eParamCurveDest3,

    eParamCurveTrig0,
    eParamCurveTrig1,
    eParamCurveTrig2,
    eParamCurveTrig3,
    
    eParamFlag0,
    eParamFlag1,
    eParamFlag2,
    eParamFlag3,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc *desc);

#endif
