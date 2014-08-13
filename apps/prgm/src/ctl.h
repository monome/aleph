//ctl.h
//aleph-prgm-avr32

//RENAME to parameters.h or something..


#ifndef _ALEPH_APP_PRGM_CTL_H_
#define _ALEPH_APP_PRGM_CTL_H_

#include "types.h"

//prgm parameters

typedef enum params {
    eParamFreq0,
    eParamFreq1,
    eParamFreq2,
    eParamFreq3,
    
    eParamTranspose0,
    eParamTranspose1,
    eParamTranspose2,
    eParamTranspose3,
    
    eParamFFAmount0,
    eParamFFAmount1,
    eParamFFAmount2,
    eParamFFAmount3,
    
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
} eParam;

u8 ctl_report_parameters(void);

#endif // h guard
