#ifndef _ALEPH_MODULE_PRGM_PARAMS_H_
#define _ALEPH_MODULE_PRGM_PARAMS_H_

#include "param_common.h"

#define FIX_RADIX 16

enum params {    
    eParamOffset,
    eParamSample,
    
    eParamTrig0,
    eParamTrig1,
    eParamTrig2,
    eParamTrig3,
    eParamTrig4,
    eParamTrig5,
    eParamTrig6,
    eParamTrig7,
    
    eParamSetTrig0,
    eParamSetTrig1,
    eParamSetTrig2,
    eParamSetTrig3,
    eParamSetTrig4,
    eParamSetTrig5,
    eParamSetTrig6,
    eParamSetTrig7,
    
    eParamFlag0,
    eParamFlag1,
    eParamFlag2,
    eParamFlag3,
    eParamFlag4,
    eParamFlag5,
    eParamFlag6,
    eParamFlag7,
    
    eParamMuteFlag0,
    eParamMuteFlag1,
    eParamMuteFlag2,
    eParamMuteFlag3,
    eParamMuteFlag4,
    eParamMuteFlag5,
    eParamMuteFlag6,
    eParamMuteFlag7,
    
    eParamMute,
    eParamUnMute,
    
    eParamCurve0,
    eParamCurve1,
    eParamCurve2,
    eParamCurve3,
    eParamCurve4,
    eParamCurve5,
    eParamCurve6,
    eParamCurve7,
    
    eParamLength,
        
    eParamInput0,
    eParamInput1,
    eParamInput2,
    eParamInput3,
    eParamInput4,
    eParamInput5,
    eParamInput6,
    eParamInput7,
    
    eParamInputLevel0,
    eParamInputLevel1,
    eParamInputLevel2,
    eParamInputLevel3,
    eParamInputLevel4,
    eParamInputLevel5,
    eParamInputLevel6,
    eParamInputLevel7,
                
    eParamOffsetA0,
    eParamOffsetA1,
    eParamOffsetA2,
    eParamOffsetA3,
    eParamOffsetA4,
    eParamOffsetA5,
    eParamOffsetA6,
    eParamOffsetA7,
    
    eParamOffsetB0,
    eParamOffsetB1,
    eParamOffsetB2,
    eParamOffsetB3,
    eParamOffsetB4,
    eParamOffsetB5,
    eParamOffsetB6,
    eParamOffsetB7,
    
    eParamSampleOffsetA0,
    eParamSampleOffsetA1,
    eParamSampleOffsetA2,
    eParamSampleOffsetA3,
    eParamSampleOffsetA4,
    eParamSampleOffsetA5,
    eParamSampleOffsetA6,
    eParamSampleOffsetA7,

    eParamSampleOffsetB0,
    eParamSampleOffsetB1,
    eParamSampleOffsetB2,
    eParamSampleOffsetB3,
    eParamSampleOffsetB4,
    eParamSampleOffsetB5,
    eParamSampleOffsetB6,
    eParamSampleOffsetB7,
    
    eParamUnsigned0,
    eParamUnsigned1,
    eParamUnsigned2,
    eParamUnsigned3,
    eParamUnsigned4,
    eParamUnsigned5,
    eParamUnsigned6,
    eParamUnsigned7,
    
    eParamSigned0,
    eParamSigned1,
    eParamSigned2,
    eParamSigned3,
    eParamSigned4,
    eParamSigned5,
    eParamSigned6,
    eParamSigned7,

    eParamOutputLevel0,
    eParamOutputLevel1,
    eParamOutputLevel2,
    eParamOutputLevel3,
    eParamOutputLevel4,
    eParamOutputLevel5,
    eParamOutputLevel6,
    eParamOutputLevel7,

    eParamRouteMix,
    
    eParamMixLevel0,
    eParamMixLevel1,
    eParamMixLevel2,
    eParamMixLevel3,
    eParamMixLevel4,
    eParamMixLevel5,
    eParamMixLevel6,
    eParamMixLevel7,
    
    eParamAuxLevel0,
    eParamAuxLevel1,
    eParamAuxLevel2,
    eParamAuxLevel3,
    eParamAuxLevel4,
    eParamAuxLevel5,
    eParamAuxLevel6,
    eParamAuxLevel7,
       
    eParamDirectOut3,
    eParamDirectOut4,

    eParamGroup1,
    eParamGroup2,
    eParamMaster,
    
    eParamResetCounter,
    eParamBPM,
    
    eParamMotor0,
    eParamMotor1,
    eParamMotor2,
    eParamMotor3,
    eParamMotor4,
    eParamMotor5,
    eParamMotor6,
    eParamMotor7,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc *desc);

#endif
