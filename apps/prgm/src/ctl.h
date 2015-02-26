//ctl.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_CTL_H_
#define _ALEPH_APP_PRGM_CTL_H_

#include <string.h>

//asf
#include "delay.h"
#include "print_funcs.h"

//avr32
#include "app.h"
#include "bfin.h"
#include "control.h"
#include "interrupts.h"

//common
#include "fix.h"
#include "types.h"
#include "param_common.h"

#define DSP_NAME "aleph-prgm"

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
    
    eParamPosition0,
    eParamPosition1,
    eParamPosition2,
    eParamPosition3,
    
    eParamLoop0,
    eParamLoop1,
    eParamLoop2,
    eParamLoop3,

    //  global parameters
    eParamInputA0,
    eParamInputA1,
    eParamInputA2,
    eParamInputA3,
    
    eParamInputB0,
    eParamInputB1,
    eParamInputB2,
    eParamInputB3,
    
    eParamMix0,
    eParamMix1,
    eParamMix2,
    eParamMix3,

    eParamLevel0,
    eParamLevel1,
    eParamLevel2,
    eParamLevel3,
    
    eParamFrequency0,
    eParamFrequency1,
    eParamFrequency2,
    eParamFrequency3,
    
    eParamOffset,
    eParamSample,
    
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
