//ctl.h
//aleph-prgm-avr32

//RENAME to parameters.h or something..


#ifndef _ALEPH_APP_PRGM_CTL_H_
#define _ALEPH_APP_PRGM_CTL_H_

#include "types.h"

//prgm parameters

typedef enum params {
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
} eParam;

u8 ctl_report_parameters(void);

#endif // h guard
