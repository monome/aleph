//ctl.h
//aleph-prgm-avr32

//RENAME to parameters.h or something..


#ifndef _ALEPH_APP_PRGM_CTL_H_
#define _ALEPH_APP_PRGM_CTL_H_

#include "types.h"

//prgm parameters

typedef enum params {
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
} eParam;

u8 ctl_report_parameters(void);

#endif // h guard
