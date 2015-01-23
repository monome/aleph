//ctl.h
//aleph-prgm-avr32

//RENAME to parameters.h or something..


#ifndef _ALEPH_APP_PRGM_CTL_H_
#define _ALEPH_APP_PRGM_CTL_H_

#include "types.h"

//prgm parameters

typedef enum params {
    //  sequenced parameters
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
    
    eParamFrequency0,
    eParamFrequency1,
    eParamFrequency2,
    eParamFrequency3,
    
    eParamFrames,
    
    //  global parameters
    eParamBufferLength0,
    eParamBufferLength1,
    eParamBufferLength2,
    eParamBufferLength3,
    
    eParamToggleBuf,
    eParamToggleRec,
    
    eParamSqCounter,
    eParamSqLength,
    
    eParamFrameFwd,
    eParamFrameBwd,
    
    eParamToggleMotor,
    
    eParamNumParams
} eParam;

u8 ctl_report_parameters(void);

#endif // h guard
