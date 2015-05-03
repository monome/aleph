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
    
    eParamCurve0,
    eParamCurve1,
    eParamCurve2,
    eParamCurve3,
    eParamCurve4,
    eParamCurve5,
    eParamCurve6,
    eParamCurve7,
    
    eParamLength0,
    eParamLength1,
    eParamLength2,
    eParamLength3,
    eParamLength4,
    eParamLength5,
    eParamLength6,
    eParamLength7,
        
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
    
    eParamMixLevel0,
    eParamMixLevel1,
    eParamMixLevel2,
    eParamMixLevel3,
    eParamMixLevel4,
    eParamMixLevel5,
    eParamMixLevel6,
    eParamMixLevel7,
        
    eParamMixPanL0,
    eParamMixPanL1,
    eParamMixPanL2,
    eParamMixPanL3,
    eParamMixPanL4,
    eParamMixPanL5,
    eParamMixPanL6,
    eParamMixPanL7,
    
    eParamMixPanR0,
    eParamMixPanR1,
    eParamMixPanR2,
    eParamMixPanR3,
    eParamMixPanR4,
    eParamMixPanR5,
    eParamMixPanR6,
    eParamMixPanR7,
        
    eParamAux1Level0,
    eParamAux1Level1,
    eParamAux1Level2,
    eParamAux1Level3,
    eParamAux1Level4,
    eParamAux1Level5,
    eParamAux1Level6,
    eParamAux1Level7,
    
    eParamAux2Level0,
    eParamAux2Level1,
    eParamAux2Level2,
    eParamAux2Level3,
    eParamAux2Level4,
    eParamAux2Level5,
    eParamAux2Level6,
    eParamAux2Level7,
    
    eParamAux1PanL,
    eParamAux1PanR,
    eParamAux2PanL,
    eParamAux2PanR,
    
    eParamDirectOut3,
    eParamDirectOut4,
    
    eParamMaster,
    
    eParamCounter,
    
    eParamNumParams
};

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(ParamDesc *desc);

#endif
