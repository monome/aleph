//prgm
//aleph-avr32

#ifndef _ALEPH_APP_PRGM_CTL_H_
#define _ALEPH_APP_PRGM_CTL_H_

#include "types.h"

//prgm parameters

typedef enum params {
    eParamFreq0,
    eParamFreq1,
    eParamFreq2,
    eParamFreq3,
    
    eParamFreqFine0,
    eParamFreqFine1,
    eParamFreqFine2,
    eParamFreqFine3,
    
    eParamFFAmount0,
    eParamFFAmount1,
    eParamFFAmount2,
    eParamFFAmount3,
    
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
    
    eParamNumParams
} eParam;

u8 ctl_report_parameters(void);

#endif // h guard
