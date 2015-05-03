
#include <string.h>
#include "param_common.h"
#include "params.h" 


void fill_param_desc(ParamDesc *desc) {
//*/
    //  Offset
    strcpy(desc[eParamOffset].label, "offset");
    desc[eParamOffset].type = eParamTypeFix;
    desc[eParamOffset].min = 0;
    desc[eParamOffset].max = FR32_MAX;
    desc[eParamOffset].radix = FIX_RADIX;
    
    //  Sample
    strcpy(desc[eParamSample].label, "sample");
    desc[eParamSample].type = eParamTypeFix;
    desc[eParamSample].min = 0;
    desc[eParamSample].max = FR32_MAX;
    desc[eParamSample].radix = FIX_RADIX;

    //  Trig
    strcpy(desc[eParamTrig0].label, "trig0");
    desc[eParamTrig0].type = eParamTypeFix;
    desc[eParamTrig0].min = 0;
    desc[eParamTrig0].max = FR32_MAX;
    desc[eParamTrig0].radix = FIX_RADIX;

    strcpy(desc[eParamTrig1].label, "trig1");
    desc[eParamTrig1].type = eParamTypeFix;
    desc[eParamTrig1].min = 0;
    desc[eParamTrig1].max = FR32_MAX;
    desc[eParamTrig1].radix = FIX_RADIX;

    strcpy(desc[eParamTrig2].label, "trig2");
    desc[eParamTrig2].type = eParamTypeFix;
    desc[eParamTrig2].min = 0;
    desc[eParamTrig2].max = FR32_MAX;
    desc[eParamTrig2].radix = FIX_RADIX;

    strcpy(desc[eParamTrig3].label, "trig3");
    desc[eParamTrig3].type = eParamTypeFix;
    desc[eParamTrig3].min = 0;
    desc[eParamTrig3].max = FR32_MAX;
    desc[eParamTrig3].radix = FIX_RADIX;
    
    strcpy(desc[eParamTrig4].label, "trig4");
    desc[eParamTrig4].type = eParamTypeFix;
    desc[eParamTrig4].min = 0;
    desc[eParamTrig4].max = FR32_MAX;
    desc[eParamTrig4].radix = FIX_RADIX;

    strcpy(desc[eParamTrig5].label, "trig5");
    desc[eParamTrig5].type = eParamTypeFix;
    desc[eParamTrig5].min = 0;
    desc[eParamTrig5].max = FR32_MAX;
    desc[eParamTrig5].radix = FIX_RADIX;

    strcpy(desc[eParamTrig6].label, "trig6");
    desc[eParamTrig6].type = eParamTypeFix;
    desc[eParamTrig6].min = 0;
    desc[eParamTrig6].max = FR32_MAX;
    desc[eParamTrig6].radix = FIX_RADIX;

    strcpy(desc[eParamTrig7].label, "trig7");
    desc[eParamTrig7].type = eParamTypeFix;
    desc[eParamTrig7].min = 0;
    desc[eParamTrig7].max = FR32_MAX;
    desc[eParamTrig7].radix = FIX_RADIX;
    
    //  Set Trig
    strcpy(desc[eParamSetTrig0].label, "sqtrig0");
    desc[eParamSetTrig0].type = eParamTypeFix;
    desc[eParamSetTrig0].min = 0;
    desc[eParamSetTrig0].max = FR32_MAX;
    desc[eParamSetTrig0].radix = FIX_RADIX;
    
    strcpy(desc[eParamSetTrig1].label, "sqtrig1");
    desc[eParamSetTrig1].type = eParamTypeFix;
    desc[eParamSetTrig1].min = 0;
    desc[eParamSetTrig1].max = FR32_MAX;
    desc[eParamSetTrig1].radix = FIX_RADIX;

    strcpy(desc[eParamSetTrig2].label, "sqtrig2");
    desc[eParamSetTrig2].type = eParamTypeFix;
    desc[eParamSetTrig2].min = 0;
    desc[eParamSetTrig2].max = FR32_MAX;
    desc[eParamSetTrig2].radix = FIX_RADIX;

    strcpy(desc[eParamSetTrig3].label, "sqtrig3");
    desc[eParamSetTrig3].type = eParamTypeFix;
    desc[eParamSetTrig3].min = 0;
    desc[eParamSetTrig3].max = FR32_MAX;
    desc[eParamSetTrig3].radix = FIX_RADIX;

    strcpy(desc[eParamSetTrig4].label, "sqtrig4");
    desc[eParamSetTrig4].type = eParamTypeFix;
    desc[eParamSetTrig4].min = 0;
    desc[eParamSetTrig4].max = FR32_MAX;
    desc[eParamSetTrig4].radix = FIX_RADIX;

    strcpy(desc[eParamSetTrig5].label, "sqtrig5");
    desc[eParamSetTrig5].type = eParamTypeFix;
    desc[eParamSetTrig5].min = 0;
    desc[eParamSetTrig5].max = FR32_MAX;
    desc[eParamSetTrig5].radix = FIX_RADIX;

    strcpy(desc[eParamSetTrig6].label, "sqtrig6");
    desc[eParamSetTrig6].type = eParamTypeFix;
    desc[eParamSetTrig6].min = 0;
    desc[eParamSetTrig6].max = FR32_MAX;
    desc[eParamSetTrig6].radix = FIX_RADIX;

    strcpy(desc[eParamSetTrig7].label, "sqtrig7");
    desc[eParamSetTrig7].type = eParamTypeFix;
    desc[eParamSetTrig7].min = 0;
    desc[eParamSetTrig7].max = FR32_MAX;
    desc[eParamSetTrig7].radix = FIX_RADIX;

    //  Flag
    strcpy(desc[eParamFlag0].label, "flag0");
    desc[eParamFlag0].type = eParamTypeFix;
    desc[eParamFlag0].min = 0;
    desc[eParamFlag0].max = FR32_MAX;
    desc[eParamFlag0].radix = FIX_RADIX;
    
    strcpy(desc[eParamFlag1].label, "flag1");
    desc[eParamFlag1].type = eParamTypeFix;
    desc[eParamFlag1].min = 0;
    desc[eParamFlag1].max = FR32_MAX;
    desc[eParamFlag1].radix = FIX_RADIX;
    
    strcpy(desc[eParamFlag2].label, "flag2");
    desc[eParamFlag2].type = eParamTypeFix;
    desc[eParamFlag2].min = 0;
    desc[eParamFlag2].max = FR32_MAX;
    desc[eParamFlag2].radix = FIX_RADIX;
    
    strcpy(desc[eParamFlag3].label, "flag3");
    desc[eParamFlag3].type = eParamTypeFix;
    desc[eParamFlag3].min = 0;
    desc[eParamFlag3].max = FR32_MAX;
    desc[eParamFlag3].radix = FIX_RADIX;
    
    strcpy(desc[eParamFlag4].label, "flag4");
    desc[eParamFlag4].type = eParamTypeFix;
    desc[eParamFlag4].min = 0;
    desc[eParamFlag4].max = FR32_MAX;
    desc[eParamFlag4].radix = FIX_RADIX;

    strcpy(desc[eParamFlag5].label, "flag5");
    desc[eParamFlag5].type = eParamTypeFix;
    desc[eParamFlag5].min = 0;
    desc[eParamFlag5].max = FR32_MAX;
    desc[eParamFlag5].radix = FIX_RADIX;

    strcpy(desc[eParamFlag6].label, "flag6");
    desc[eParamFlag6].type = eParamTypeFix;
    desc[eParamFlag6].min = 0;
    desc[eParamFlag6].max = FR32_MAX;
    desc[eParamFlag6].radix = FIX_RADIX;

    strcpy(desc[eParamFlag7].label, "flag7");
    desc[eParamFlag7].type = eParamTypeFix;
    desc[eParamFlag7].min = 0;
    desc[eParamFlag7].max = FR32_MAX;
    desc[eParamFlag7].radix = FIX_RADIX;

    //  Curve
    strcpy(desc[eParamCurve0].label, "curve0");
    desc[eParamCurve0].type = eParamTypeFix;
    desc[eParamCurve0].min = 0;
    desc[eParamCurve0].max = FR32_MAX;
    desc[eParamCurve0].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve1].label, "curve1");
    desc[eParamCurve1].type = eParamTypeFix;
    desc[eParamCurve1].min = 0;
    desc[eParamCurve1].max = FR32_MAX;
    desc[eParamCurve1].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve2].label, "curve2");
    desc[eParamCurve2].type = eParamTypeFix;
    desc[eParamCurve2].min = 0;
    desc[eParamCurve2].max = FR32_MAX;
    desc[eParamCurve2].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve3].label, "curve3");
    desc[eParamCurve3].type = eParamTypeFix;
    desc[eParamCurve3].min = 0;
    desc[eParamCurve3].max = FR32_MAX;
    desc[eParamCurve3].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve4].label, "curve4");
    desc[eParamCurve4].type = eParamTypeFix;
    desc[eParamCurve4].min = 0;
    desc[eParamCurve4].max = FR32_MAX;
    desc[eParamCurve4].radix = FIX_RADIX;

    strcpy(desc[eParamCurve5].label, "curve5");
    desc[eParamCurve5].type = eParamTypeFix;
    desc[eParamCurve5].min = 0;
    desc[eParamCurve5].max = FR32_MAX;
    desc[eParamCurve5].radix = FIX_RADIX;

    strcpy(desc[eParamCurve6].label, "curve6");
    desc[eParamCurve6].type = eParamTypeFix;
    desc[eParamCurve6].min = 0;
    desc[eParamCurve6].max = FR32_MAX;
    desc[eParamCurve6].radix = FIX_RADIX;

    strcpy(desc[eParamCurve7].label, "curve7");
    desc[eParamCurve7].type = eParamTypeFix;
    desc[eParamCurve7].min = 0;
    desc[eParamCurve7].max = FR32_MAX;
    desc[eParamCurve7].radix = FIX_RADIX;
    
    //Length
    strcpy(desc[eParamLength0].label, "length0");
    desc[eParamLength0].type = eParamTypeFix;
    desc[eParamLength0].min = 0;
    desc[eParamLength0].max = FR32_MAX;
    desc[eParamLength0].radix = FIX_RADIX;

    strcpy(desc[eParamLength1].label, "length1");
    desc[eParamLength1].type = eParamTypeFix;
    desc[eParamLength1].min = 0;
    desc[eParamLength1].max = FR32_MAX;
    desc[eParamLength1].radix = FIX_RADIX;

    strcpy(desc[eParamLength2].label, "length2");
    desc[eParamLength2].type = eParamTypeFix;
    desc[eParamLength2].min = 0;
    desc[eParamLength2].max = FR32_MAX;
    desc[eParamLength2].radix = FIX_RADIX;

    strcpy(desc[eParamLength3].label, "length3");
    desc[eParamLength3].type = eParamTypeFix;
    desc[eParamLength3].min = 0;
    desc[eParamLength3].max = FR32_MAX;
    desc[eParamLength3].radix = FIX_RADIX;

    strcpy(desc[eParamLength4].label, "length4");
    desc[eParamLength4].type = eParamTypeFix;
    desc[eParamLength4].min = 0;
    desc[eParamLength4].max = FR32_MAX;
    desc[eParamLength4].radix = FIX_RADIX;

    strcpy(desc[eParamLength5].label, "length5");
    desc[eParamLength5].type = eParamTypeFix;
    desc[eParamLength5].min = 0;
    desc[eParamLength5].max = FR32_MAX;
    desc[eParamLength5].radix = FIX_RADIX;

    strcpy(desc[eParamLength6].label, "length6");
    desc[eParamLength6].type = eParamTypeFix;
    desc[eParamLength6].min = 0;
    desc[eParamLength6].max = FR32_MAX;
    desc[eParamLength6].radix = FIX_RADIX;

    strcpy(desc[eParamLength7].label, "length7");
    desc[eParamLength7].type = eParamTypeFix;
    desc[eParamLength7].min = 0;
    desc[eParamLength7].max = FR32_MAX;
    desc[eParamLength7].radix = FIX_RADIX;
    
    //  Input
    strcpy(desc[eParamInput0].label, "input0");
    desc[eParamInput0].type = eParamTypeFix;
    desc[eParamInput0].min = 0;
    desc[eParamInput0].max = FR32_MAX;
    desc[eParamInput0].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput1].label, "input1");
    desc[eParamInput1].type = eParamTypeFix;
    desc[eParamInput1].min = 0;
    desc[eParamInput1].max = FR32_MAX;
    desc[eParamInput1].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput2].label, "input2");
    desc[eParamInput2].type = eParamTypeFix;
    desc[eParamInput2].min = 0;
    desc[eParamInput2].max = FR32_MAX;
    desc[eParamInput2].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput3].label, "input3");
    desc[eParamInput3].type = eParamTypeFix;
    desc[eParamInput3].min = 0;
    desc[eParamInput3].max = FR32_MAX;
    desc[eParamInput3].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput4].label, "input4");
    desc[eParamInput4].type = eParamTypeFix;
    desc[eParamInput4].min = 0;
    desc[eParamInput4].max = FR32_MAX;
    desc[eParamInput4].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput5].label, "input5");
    desc[eParamInput5].type = eParamTypeFix;
    desc[eParamInput5].min = 0;
    desc[eParamInput5].max = FR32_MAX;
    desc[eParamInput5].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput6].label, "input6");
    desc[eParamInput6].type = eParamTypeFix;
    desc[eParamInput6].min = 0;
    desc[eParamInput6].max = FR32_MAX;
    desc[eParamInput6].radix = FIX_RADIX;
    
    strcpy(desc[eParamInput7].label, "input7");
    desc[eParamInput7].type = eParamTypeFix;
    desc[eParamInput7].min = 0;
    desc[eParamInput7].max = FR32_MAX;
    desc[eParamInput7].radix = FIX_RADIX;
    
    //  Input Level
    strcpy(desc[eParamInputLevel0].label, "inputlevel0");
    desc[eParamInputLevel0].type = eParamTypeFix;
    desc[eParamInputLevel0].min = 0;
    desc[eParamInputLevel0].max = FR32_MAX;
    desc[eParamInputLevel0].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel1].label, "inputlevel1");
    desc[eParamInputLevel1].type = eParamTypeFix;
    desc[eParamInputLevel1].min = 0;
    desc[eParamInputLevel1].max = FR32_MAX;
    desc[eParamInputLevel1].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel2].label, "inputlevel2");
    desc[eParamInputLevel2].type = eParamTypeFix;
    desc[eParamInputLevel2].min = 0;
    desc[eParamInputLevel2].max = FR32_MAX;
    desc[eParamInputLevel2].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel3].label, "inputlevel3");
    desc[eParamInputLevel3].type = eParamTypeFix;
    desc[eParamInputLevel3].min = 0;
    desc[eParamInputLevel3].max = FR32_MAX;
    desc[eParamInputLevel3].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel4].label, "inputlevel4");
    desc[eParamInputLevel4].type = eParamTypeFix;
    desc[eParamInputLevel4].min = 0;
    desc[eParamInputLevel4].max = FR32_MAX;
    desc[eParamInputLevel4].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel5].label, "inputlevel5");
    desc[eParamInputLevel5].type = eParamTypeFix;
    desc[eParamInputLevel5].min = 0;
    desc[eParamInputLevel5].max = FR32_MAX;
    desc[eParamInputLevel5].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel6].label, "inputlevel6");
    desc[eParamInputLevel6].type = eParamTypeFix;
    desc[eParamInputLevel6].min = 0;
    desc[eParamInputLevel6].max = FR32_MAX;
    desc[eParamInputLevel6].radix = FIX_RADIX;

    strcpy(desc[eParamInputLevel7].label, "inputlevel7");
    desc[eParamInputLevel7].type = eParamTypeFix;
    desc[eParamInputLevel7].min = 0;
    desc[eParamInputLevel7].max = FR32_MAX;
    desc[eParamInputLevel7].radix = FIX_RADIX;
        
    //  OffsetA
    strcpy(desc[eParamOffsetA0].label, "offsetA0");
    desc[eParamOffsetA0].type = eParamTypeFix;
    desc[eParamOffsetA0].min = 0;
    desc[eParamOffsetA0].max = FR32_MAX;
    desc[eParamOffsetA0].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetA1].label, "offsetA1");
    desc[eParamOffsetA1].type = eParamTypeFix;
    desc[eParamOffsetA1].min = 0;
    desc[eParamOffsetA1].max = FR32_MAX;
    desc[eParamOffsetA1].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetA2].label, "offsetA2");
    desc[eParamOffsetA2].type = eParamTypeFix;
    desc[eParamOffsetA2].min = 0;
    desc[eParamOffsetA2].max = FR32_MAX;
    desc[eParamOffsetA2].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetA3].label, "offsetA3");
    desc[eParamOffsetA3].type = eParamTypeFix;
    desc[eParamOffsetA3].min = 0;
    desc[eParamOffsetA3].max = FR32_MAX;
    desc[eParamOffsetA3].radix = FIX_RADIX;
    
    strcpy(desc[eParamOffsetA4].label, "offsetA4");
    desc[eParamOffsetA4].type = eParamTypeFix;
    desc[eParamOffsetA4].min = 0;
    desc[eParamOffsetA4].max = FR32_MAX;
    desc[eParamOffsetA4].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetA5].label, "offsetA5");
    desc[eParamOffsetA5].type = eParamTypeFix;
    desc[eParamOffsetA5].min = 0;
    desc[eParamOffsetA5].max = FR32_MAX;
    desc[eParamOffsetA5].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetA6].label, "offsetA6");
    desc[eParamOffsetA6].type = eParamTypeFix;
    desc[eParamOffsetA6].min = 0;
    desc[eParamOffsetA6].max = FR32_MAX;
    desc[eParamOffsetA6].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetA7].label, "offsetA7");
    desc[eParamOffsetA7].type = eParamTypeFix;
    desc[eParamOffsetA7].min = 0;
    desc[eParamOffsetA7].max = FR32_MAX;
    desc[eParamOffsetA7].radix = FIX_RADIX;
    
    //  OffsetB
    strcpy(desc[eParamOffsetB0].label, "offsetB0");
    desc[eParamOffsetB0].type = eParamTypeFix;
    desc[eParamOffsetB0].min = 0;
    desc[eParamOffsetB0].max = FR32_MAX;
    desc[eParamOffsetB0].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB1].label, "offsetB1");
    desc[eParamOffsetB1].type = eParamTypeFix;
    desc[eParamOffsetB1].min = 0;
    desc[eParamOffsetB1].max = FR32_MAX;
    desc[eParamOffsetB1].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB2].label, "offsetB2");
    desc[eParamOffsetB2].type = eParamTypeFix;
    desc[eParamOffsetB2].min = 0;
    desc[eParamOffsetB2].max = FR32_MAX;
    desc[eParamOffsetB2].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB3].label, "offsetB3");
    desc[eParamOffsetB3].type = eParamTypeFix;
    desc[eParamOffsetB3].min = 0;
    desc[eParamOffsetB3].max = FR32_MAX;
    desc[eParamOffsetB3].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB4].label, "offsetB4");
    desc[eParamOffsetB4].type = eParamTypeFix;
    desc[eParamOffsetB4].min = 0;
    desc[eParamOffsetB4].max = FR32_MAX;
    desc[eParamOffsetB4].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB5].label, "offsetB5");
    desc[eParamOffsetB5].type = eParamTypeFix;
    desc[eParamOffsetB5].min = 0;
    desc[eParamOffsetB5].max = FR32_MAX;
    desc[eParamOffsetB5].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB6].label, "offsetB6");
    desc[eParamOffsetB6].type = eParamTypeFix;
    desc[eParamOffsetB6].min = 0;
    desc[eParamOffsetB6].max = FR32_MAX;
    desc[eParamOffsetB6].radix = FIX_RADIX;

    strcpy(desc[eParamOffsetB7].label, "offsetB7");
    desc[eParamOffsetB7].type = eParamTypeFix;
    desc[eParamOffsetB7].min = 0;
    desc[eParamOffsetB7].max = FR32_MAX;
    desc[eParamOffsetB7].radix = FIX_RADIX;
    
    //  SampleOffsetA
    strcpy(desc[eParamSampleOffsetA0].label, "sampleA0");
    desc[eParamSampleOffsetA0].type = eParamTypeFix;
    desc[eParamSampleOffsetA0].min = 0;
    desc[eParamSampleOffsetA0].max = FR32_MAX;
    desc[eParamSampleOffsetA0].radix = FIX_RADIX;
    
    strcpy(desc[eParamSampleOffsetA1].label, "sampleA1");
    desc[eParamSampleOffsetA1].type = eParamTypeFix;
    desc[eParamSampleOffsetA1].min = 0;
    desc[eParamSampleOffsetA1].max = FR32_MAX;
    desc[eParamSampleOffsetA1].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetA2].label, "sampleA2");
    desc[eParamSampleOffsetA2].type = eParamTypeFix;
    desc[eParamSampleOffsetA2].min = 0;
    desc[eParamSampleOffsetA2].max = FR32_MAX;
    desc[eParamSampleOffsetA2].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetA3].label, "sampleA3");
    desc[eParamSampleOffsetA3].type = eParamTypeFix;
    desc[eParamSampleOffsetA3].min = 0;
    desc[eParamSampleOffsetA3].max = FR32_MAX;
    desc[eParamSampleOffsetA3].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetA4].label, "sampleA4");
    desc[eParamSampleOffsetA4].type = eParamTypeFix;
    desc[eParamSampleOffsetA4].min = 0;
    desc[eParamSampleOffsetA4].max = FR32_MAX;
    desc[eParamSampleOffsetA4].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetA5].label, "sampleA5");
    desc[eParamSampleOffsetA5].type = eParamTypeFix;
    desc[eParamSampleOffsetA5].min = 0;
    desc[eParamSampleOffsetA5].max = FR32_MAX;
    desc[eParamSampleOffsetA5].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetA6].label, "sampleA6");
    desc[eParamSampleOffsetA6].type = eParamTypeFix;
    desc[eParamSampleOffsetA6].min = 0;
    desc[eParamSampleOffsetA6].max = FR32_MAX;
    desc[eParamSampleOffsetA6].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetA7].label, "sampleA7");
    desc[eParamSampleOffsetA7].type = eParamTypeFix;
    desc[eParamSampleOffsetA7].min = 0;
    desc[eParamSampleOffsetA7].max = FR32_MAX;
    desc[eParamSampleOffsetA7].radix = FIX_RADIX;

    //  SampleOffsetB
    strcpy(desc[eParamSampleOffsetB0].label, "sampleB0");
    desc[eParamSampleOffsetB0].type = eParamTypeFix;
    desc[eParamSampleOffsetB0].min = 0;
    desc[eParamSampleOffsetB0].max = FR32_MAX;
    desc[eParamSampleOffsetB0].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB1].label, "sampleB1");
    desc[eParamSampleOffsetB1].type = eParamTypeFix;
    desc[eParamSampleOffsetB1].min = 0;
    desc[eParamSampleOffsetB1].max = FR32_MAX;
    desc[eParamSampleOffsetB1].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB2].label, "sampleB2");
    desc[eParamSampleOffsetB2].type = eParamTypeFix;
    desc[eParamSampleOffsetB2].min = 0;
    desc[eParamSampleOffsetB2].max = FR32_MAX;
    desc[eParamSampleOffsetB2].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB3].label, "sampleB3");
    desc[eParamSampleOffsetB3].type = eParamTypeFix;
    desc[eParamSampleOffsetB3].min = 0;
    desc[eParamSampleOffsetB3].max = FR32_MAX;
    desc[eParamSampleOffsetB3].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB4].label, "sampleB4");
    desc[eParamSampleOffsetB4].type = eParamTypeFix;
    desc[eParamSampleOffsetB4].min = 0;
    desc[eParamSampleOffsetB4].max = FR32_MAX;
    desc[eParamSampleOffsetB4].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB5].label, "sampleB5");
    desc[eParamSampleOffsetB5].type = eParamTypeFix;
    desc[eParamSampleOffsetB5].min = 0;
    desc[eParamSampleOffsetB5].max = FR32_MAX;
    desc[eParamSampleOffsetB5].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB6].label, "sampleB6");
    desc[eParamSampleOffsetB6].type = eParamTypeFix;
    desc[eParamSampleOffsetB6].min = 0;
    desc[eParamSampleOffsetB6].max = FR32_MAX;
    desc[eParamSampleOffsetB6].radix = FIX_RADIX;

    strcpy(desc[eParamSampleOffsetB7].label, "sampleB7");
    desc[eParamSampleOffsetB7].type = eParamTypeFix;
    desc[eParamSampleOffsetB7].min = 0;
    desc[eParamSampleOffsetB7].max = FR32_MAX;
    desc[eParamSampleOffsetB7].radix = FIX_RADIX;

    //  Unsigned (process frame)
    strcpy(desc[eParamUnsigned0].label, "unsigned0");
    desc[eParamUnsigned0].type = eParamTypeFix;
    desc[eParamUnsigned0].min = 0;
    desc[eParamUnsigned0].max = FR32_MAX;
    desc[eParamUnsigned0].radix = FIX_RADIX;

    strcpy(desc[eParamUnsigned1].label, "unsigned1");
    desc[eParamUnsigned1].type = eParamTypeFix;
    desc[eParamUnsigned1].min = 0;
    desc[eParamUnsigned1].max = FR32_MAX;
    desc[eParamUnsigned1].radix = FIX_RADIX;
    
    strcpy(desc[eParamUnsigned2].label, "unsigned2");
    desc[eParamUnsigned2].type = eParamTypeFix;
    desc[eParamUnsigned2].min = 0;
    desc[eParamUnsigned2].max = FR32_MAX;
    desc[eParamUnsigned2].radix = FIX_RADIX;

    strcpy(desc[eParamUnsigned3].label, "unsigned3");
    desc[eParamUnsigned3].type = eParamTypeFix;
    desc[eParamUnsigned3].min = 0;
    desc[eParamUnsigned3].max = FR32_MAX;
    desc[eParamUnsigned3].radix = FIX_RADIX;

    strcpy(desc[eParamUnsigned4].label, "unsigned4");
    desc[eParamUnsigned4].type = eParamTypeFix;
    desc[eParamUnsigned4].min = 0;
    desc[eParamUnsigned4].max = FR32_MAX;
    desc[eParamUnsigned4].radix = FIX_RADIX;

    strcpy(desc[eParamUnsigned5].label, "unsigned5");
    desc[eParamUnsigned5].type = eParamTypeFix;
    desc[eParamUnsigned5].min = 0;
    desc[eParamUnsigned5].max = FR32_MAX;
    desc[eParamUnsigned5].radix = FIX_RADIX;

    strcpy(desc[eParamUnsigned6].label, "unsigned6");
    desc[eParamUnsigned6].type = eParamTypeFix;
    desc[eParamUnsigned6].min = 0;
    desc[eParamUnsigned6].max = FR32_MAX;
    desc[eParamUnsigned6].radix = FIX_RADIX;

    strcpy(desc[eParamUnsigned7].label, "unsigned7");
    desc[eParamUnsigned7].type = eParamTypeFix;
    desc[eParamUnsigned7].min = 0;
    desc[eParamUnsigned7].max = FR32_MAX;
    desc[eParamUnsigned7].radix = FIX_RADIX;

    //  Signed (process frame)
    strcpy(desc[eParamSigned0].label, "signed0");
    desc[eParamSigned0].type = eParamTypeFix;
    desc[eParamSigned0].min = 0;
    desc[eParamSigned0].max = FR32_MAX;
    desc[eParamSigned0].radix = FIX_RADIX;

    strcpy(desc[eParamSigned1].label, "signed1");
    desc[eParamSigned1].type = eParamTypeFix;
    desc[eParamSigned1].min = 0;
    desc[eParamSigned1].max = FR32_MAX;
    desc[eParamSigned1].radix = FIX_RADIX;

    strcpy(desc[eParamSigned2].label, "signed2");
    desc[eParamSigned2].type = eParamTypeFix;
    desc[eParamSigned2].min = 0;
    desc[eParamSigned2].max = FR32_MAX;
    desc[eParamSigned2].radix = FIX_RADIX;

    strcpy(desc[eParamSigned3].label, "signed3");
    desc[eParamSigned3].type = eParamTypeFix;
    desc[eParamSigned3].min = 0;
    desc[eParamSigned3].max = FR32_MAX;
    desc[eParamSigned3].radix = FIX_RADIX;

    strcpy(desc[eParamSigned4].label, "signed4");
    desc[eParamSigned4].type = eParamTypeFix;
    desc[eParamSigned4].min = 0;
    desc[eParamSigned4].max = FR32_MAX;
    desc[eParamSigned4].radix = FIX_RADIX;

    strcpy(desc[eParamSigned5].label, "signed5");
    desc[eParamSigned5].type = eParamTypeFix;
    desc[eParamSigned5].min = 0;
    desc[eParamSigned5].max = FR32_MAX;
    desc[eParamSigned5].radix = FIX_RADIX;

    strcpy(desc[eParamSigned6].label, "signed6");
    desc[eParamSigned6].type = eParamTypeFix;
    desc[eParamSigned6].min = 0;
    desc[eParamSigned6].max = FR32_MAX;
    desc[eParamSigned6].radix = FIX_RADIX;

    strcpy(desc[eParamSigned7].label, "signed7");
    desc[eParamSigned7].type = eParamTypeFix;
    desc[eParamSigned7].min = 0;
    desc[eParamSigned7].max = FR32_MAX;
    desc[eParamSigned7].radix = FIX_RADIX;
    
    //  Output Level
    strcpy(desc[eParamOutputLevel0].label, "level0");
    desc[eParamOutputLevel0].type = eParamTypeFix;
    desc[eParamOutputLevel0].min = 0;
    desc[eParamOutputLevel0].max = FR32_MAX;
    desc[eParamOutputLevel0].radix = FIX_RADIX;
    
    strcpy(desc[eParamOutputLevel1].label, "level1");
    desc[eParamOutputLevel1].type = eParamTypeFix;
    desc[eParamOutputLevel1].min = 0;
    desc[eParamOutputLevel1].max = FR32_MAX;
    desc[eParamOutputLevel1].radix = FIX_RADIX;
    
    strcpy(desc[eParamOutputLevel2].label, "level2");
    desc[eParamOutputLevel2].type = eParamTypeFix;
    desc[eParamOutputLevel2].min = 0;
    desc[eParamOutputLevel2].max = FR32_MAX;
    desc[eParamOutputLevel2].radix = FIX_RADIX;
    
    strcpy(desc[eParamOutputLevel3].label, "level3");
    desc[eParamOutputLevel3].type = eParamTypeFix;
    desc[eParamOutputLevel3].min = 0;
    desc[eParamOutputLevel3].max = FR32_MAX;
    desc[eParamOutputLevel3].radix = FIX_RADIX;
    
    strcpy(desc[eParamOutputLevel4].label, "level4");
    desc[eParamOutputLevel4].type = eParamTypeFix;
    desc[eParamOutputLevel4].min = 0;
    desc[eParamOutputLevel4].max = FR32_MAX;
    desc[eParamOutputLevel4].radix = FIX_RADIX;

    strcpy(desc[eParamOutputLevel5].label, "level5");
    desc[eParamOutputLevel5].type = eParamTypeFix;
    desc[eParamOutputLevel5].min = 0;
    desc[eParamOutputLevel5].max = FR32_MAX;
    desc[eParamOutputLevel5].radix = FIX_RADIX;

    strcpy(desc[eParamOutputLevel6].label, "level6");
    desc[eParamOutputLevel6].type = eParamTypeFix;
    desc[eParamOutputLevel6].min = 0;
    desc[eParamOutputLevel6].max = FR32_MAX;
    desc[eParamOutputLevel6].radix = FIX_RADIX;

    strcpy(desc[eParamOutputLevel7].label, "level7");
    desc[eParamOutputLevel7].type = eParamTypeFix;
    desc[eParamOutputLevel7].min = 0;
    desc[eParamOutputLevel7].max = FR32_MAX;
    desc[eParamOutputLevel7].radix = FIX_RADIX;

    //  Mix Level
    strcpy(desc[eParamMixLevel0].label, "mixlevel0");
    desc[eParamMixLevel0].type = eParamTypeFix;
    desc[eParamMixLevel0].min = 0;
    desc[eParamMixLevel0].max = FR32_MAX;
    desc[eParamMixLevel0].radix = FIX_RADIX;
    
    strcpy(desc[eParamMixLevel1].label, "mixlevel1");
    desc[eParamMixLevel1].type = eParamTypeFix;
    desc[eParamMixLevel1].min = 0;
    desc[eParamMixLevel1].max = FR32_MAX;
    desc[eParamMixLevel1].radix = FIX_RADIX;

    strcpy(desc[eParamMixLevel2].label, "mixlevel2");
    desc[eParamMixLevel2].type = eParamTypeFix;
    desc[eParamMixLevel2].min = 0;
    desc[eParamMixLevel2].max = FR32_MAX;
    desc[eParamMixLevel2].radix = FIX_RADIX;

    strcpy(desc[eParamMixLevel3].label, "mixlevel3");
    desc[eParamMixLevel3].type = eParamTypeFix;
    desc[eParamMixLevel3].min = 0;
    desc[eParamMixLevel3].max = FR32_MAX;
    desc[eParamMixLevel3].radix = FIX_RADIX;

    strcpy(desc[eParamMixLevel4].label, "mixlevel4");
    desc[eParamMixLevel4].type = eParamTypeFix;
    desc[eParamMixLevel4].min = 0;
    desc[eParamMixLevel4].max = FR32_MAX;
    desc[eParamMixLevel4].radix = FIX_RADIX;

    strcpy(desc[eParamMixLevel5].label, "mixlevel5");
    desc[eParamMixLevel5].type = eParamTypeFix;
    desc[eParamMixLevel5].min = 0;
    desc[eParamMixLevel5].max = FR32_MAX;
    desc[eParamMixLevel5].radix = FIX_RADIX;

    strcpy(desc[eParamMixLevel6].label, "mixlevel6");
    desc[eParamMixLevel6].type = eParamTypeFix;
    desc[eParamMixLevel6].min = 0;
    desc[eParamMixLevel6].max = FR32_MAX;
    desc[eParamMixLevel6].radix = FIX_RADIX;

    strcpy(desc[eParamMixLevel7].label, "mixlevel7");
    desc[eParamMixLevel7].type = eParamTypeFix;
    desc[eParamMixLevel7].min = 0;
    desc[eParamMixLevel7].max = FR32_MAX;
    desc[eParamMixLevel7].radix = FIX_RADIX;
    
    //  Mix Pan
    strcpy(desc[eParamMixPanL0].label, "mixpanL0");
    desc[eParamMixPanL0].type = eParamTypeFix;
    desc[eParamMixPanL0].min = 0;
    desc[eParamMixPanL0].max = FR32_MAX;
    desc[eParamMixPanL0].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL1].label, "mixpanL1");
    desc[eParamMixPanL1].type = eParamTypeFix;
    desc[eParamMixPanL1].min = 0;
    desc[eParamMixPanL1].max = FR32_MAX;
    desc[eParamMixPanL1].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL2].label, "mixpanL2");
    desc[eParamMixPanL2].type = eParamTypeFix;
    desc[eParamMixPanL2].min = 0;
    desc[eParamMixPanL2].max = FR32_MAX;
    desc[eParamMixPanL2].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL3].label, "mixpanL3");
    desc[eParamMixPanL3].type = eParamTypeFix;
    desc[eParamMixPanL3].min = 0;
    desc[eParamMixPanL3].max = FR32_MAX;
    desc[eParamMixPanL3].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL4].label, "mixpanL4");
    desc[eParamMixPanL4].type = eParamTypeFix;
    desc[eParamMixPanL4].min = 0;
    desc[eParamMixPanL4].max = FR32_MAX;
    desc[eParamMixPanL4].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL5].label, "mixpanL5");
    desc[eParamMixPanL5].type = eParamTypeFix;
    desc[eParamMixPanL5].min = 0;
    desc[eParamMixPanL5].max = FR32_MAX;
    desc[eParamMixPanL5].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL6].label, "mixpanL6");
    desc[eParamMixPanL6].type = eParamTypeFix;
    desc[eParamMixPanL6].min = 0;
    desc[eParamMixPanL6].max = FR32_MAX;
    desc[eParamMixPanL6].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanL7].label, "mixpanL7");
    desc[eParamMixPanL7].type = eParamTypeFix;
    desc[eParamMixPanL7].min = 0;
    desc[eParamMixPanL7].max = FR32_MAX;
    desc[eParamMixPanL7].radix = FIX_RADIX;
    
    strcpy(desc[eParamMixPanR0].label, "mixpanR0");
    desc[eParamMixPanR0].type = eParamTypeFix;
    desc[eParamMixPanR0].min = 0;
    desc[eParamMixPanR0].max = FR32_MAX;
    desc[eParamMixPanR0].radix = FIX_RADIX;
    
    strcpy(desc[eParamMixPanR1].label, "mixpanR1");
    desc[eParamMixPanR1].type = eParamTypeFix;
    desc[eParamMixPanR1].min = 0;
    desc[eParamMixPanR1].max = FR32_MAX;
    desc[eParamMixPanR1].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanR2].label, "mixpanR2");
    desc[eParamMixPanR2].type = eParamTypeFix;
    desc[eParamMixPanR2].min = 0;
    desc[eParamMixPanR2].max = FR32_MAX;
    desc[eParamMixPanR2].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanR3].label, "mixpanR3");
    desc[eParamMixPanR3].type = eParamTypeFix;
    desc[eParamMixPanR3].min = 0;
    desc[eParamMixPanR3].max = FR32_MAX;
    desc[eParamMixPanR3].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanR4].label, "mixpanR4");
    desc[eParamMixPanR4].type = eParamTypeFix;
    desc[eParamMixPanR4].min = 0;
    desc[eParamMixPanR4].max = FR32_MAX;
    desc[eParamMixPanR4].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanR5].label, "mixpanR5");
    desc[eParamMixPanR5].type = eParamTypeFix;
    desc[eParamMixPanR5].min = 0;
    desc[eParamMixPanR5].max = FR32_MAX;
    desc[eParamMixPanR5].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanR6].label, "mixpanR6");
    desc[eParamMixPanR6].type = eParamTypeFix;
    desc[eParamMixPanR6].min = 0;
    desc[eParamMixPanR6].max = FR32_MAX;
    desc[eParamMixPanR6].radix = FIX_RADIX;

    strcpy(desc[eParamMixPanR7].label, "mixpanR7");
    desc[eParamMixPanR7].type = eParamTypeFix;
    desc[eParamMixPanR7].min = 0;
    desc[eParamMixPanR7].max = FR32_MAX;
    desc[eParamMixPanR7].radix = FIX_RADIX;
    
    //  Aux1 Level
    strcpy(desc[eParamAux1Level0].label, "aux1level0");
    desc[eParamAux1Level0].type = eParamTypeFix;
    desc[eParamAux1Level0].min = 0;
    desc[eParamAux1Level0].max = FR32_MAX;
    desc[eParamAux1Level0].radix = FIX_RADIX;

    strcpy(desc[eParamAux1Level1].label, "aux1level1");
    desc[eParamAux1Level1].type = eParamTypeFix;
    desc[eParamAux1Level1].min = 0;
    desc[eParamAux1Level1].max = FR32_MAX;
    desc[eParamAux1Level1].radix = FIX_RADIX;

    strcpy(desc[eParamAux1Level2].label, "aux1level2");
    desc[eParamAux1Level2].type = eParamTypeFix;
    desc[eParamAux1Level2].min = 0;
    desc[eParamAux1Level2].max = FR32_MAX;
    desc[eParamAux1Level2].radix = FIX_RADIX;

    strcpy(desc[eParamAux1Level3].label, "aux1level3");
    desc[eParamAux1Level3].type = eParamTypeFix;
    desc[eParamAux1Level3].min = 0;
    desc[eParamAux1Level3].max = FR32_MAX;
    desc[eParamAux1Level3].radix = FIX_RADIX;
    
    strcpy(desc[eParamAux1Level4].label, "aux1level4");
    desc[eParamAux1Level4].type = eParamTypeFix;
    desc[eParamAux1Level4].min = 0;
    desc[eParamAux1Level4].max = FR32_MAX;
    desc[eParamAux1Level4].radix = FIX_RADIX;

    strcpy(desc[eParamAux1Level5].label, "aux1level5");
    desc[eParamAux1Level5].type = eParamTypeFix;
    desc[eParamAux1Level5].min = 0;
    desc[eParamAux1Level5].max = FR32_MAX;
    desc[eParamAux1Level5].radix = FIX_RADIX;

    strcpy(desc[eParamAux1Level6].label, "aux1level6");
    desc[eParamAux1Level6].type = eParamTypeFix;
    desc[eParamAux1Level6].min = 0;
    desc[eParamAux1Level6].max = FR32_MAX;
    desc[eParamAux1Level6].radix = FIX_RADIX;

    strcpy(desc[eParamAux1Level7].label, "aux1level7");
    desc[eParamAux1Level7].type = eParamTypeFix;
    desc[eParamAux1Level7].min = 0;
    desc[eParamAux1Level7].max = FR32_MAX;
    desc[eParamAux1Level7].radix = FIX_RADIX;

    //  Aux2 Level
    strcpy(desc[eParamAux2Level0].label, "aux2level0");
    desc[eParamAux2Level0].type = eParamTypeFix;
    desc[eParamAux2Level0].min = 0;
    desc[eParamAux2Level0].max = FR32_MAX;
    desc[eParamAux2Level0].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level1].label, "aux2level1");
    desc[eParamAux2Level1].type = eParamTypeFix;
    desc[eParamAux2Level1].min = 0;
    desc[eParamAux2Level1].max = FR32_MAX;
    desc[eParamAux2Level1].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level2].label, "aux2level2");
    desc[eParamAux2Level2].type = eParamTypeFix;
    desc[eParamAux2Level2].min = 0;
    desc[eParamAux2Level2].max = FR32_MAX;
    desc[eParamAux2Level2].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level3].label, "aux2level3");
    desc[eParamAux2Level3].type = eParamTypeFix;
    desc[eParamAux2Level3].min = 0;
    desc[eParamAux2Level3].max = FR32_MAX;
    desc[eParamAux2Level3].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level4].label, "aux2level4");
    desc[eParamAux2Level4].type = eParamTypeFix;
    desc[eParamAux2Level4].min = 0;
    desc[eParamAux2Level4].max = FR32_MAX;
    desc[eParamAux2Level4].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level5].label, "aux2level5");
    desc[eParamAux2Level5].type = eParamTypeFix;
    desc[eParamAux2Level5].min = 0;
    desc[eParamAux2Level5].max = FR32_MAX;
    desc[eParamAux2Level5].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level6].label, "aux2level6");
    desc[eParamAux2Level6].type = eParamTypeFix;
    desc[eParamAux2Level6].min = 0;
    desc[eParamAux2Level6].max = FR32_MAX;
    desc[eParamAux2Level6].radix = FIX_RADIX;

    strcpy(desc[eParamAux2Level7].label, "aux2level7");
    desc[eParamAux2Level7].type = eParamTypeFix;
    desc[eParamAux2Level7].min = 0;
    desc[eParamAux2Level7].max = FR32_MAX;
    desc[eParamAux2Level7].radix = FIX_RADIX;
    
    //  Aux1 Pan
    strcpy(desc[eParamAux1PanL].label, "aux1panL");
    desc[eParamAux1PanL].type = eParamTypeFix;
    desc[eParamAux1PanL].min = 0;
    desc[eParamAux1PanL].max = FR32_MAX;
    desc[eParamAux1PanL].radix = FIX_RADIX;

    strcpy(desc[eParamAux1PanR].label, "aux1panR");
    desc[eParamAux1PanR].type = eParamTypeFix;
    desc[eParamAux1PanR].min = 0;
    desc[eParamAux1PanR].max = FR32_MAX;
    desc[eParamAux1PanR].radix = FIX_RADIX;

    //  Aux2 Pan
    strcpy(desc[eParamAux2PanL].label, "aux2panL");
    desc[eParamAux2PanL].type = eParamTypeFix;
    desc[eParamAux2PanL].min = 0;
    desc[eParamAux2PanL].max = FR32_MAX;
    desc[eParamAux2PanL].radix = FIX_RADIX;
    
    strcpy(desc[eParamAux2PanR].label, "aux2panR");
    desc[eParamAux2PanR].type = eParamTypeFix;
    desc[eParamAux2PanR].min = 0;
    desc[eParamAux2PanR].max = FR32_MAX;
    desc[eParamAux2PanR].radix = FIX_RADIX;
    
    //  Direct Out
    strcpy(desc[eParamDirectOut3].label, "directout3");
    desc[eParamDirectOut3].type = eParamTypeFix;
    desc[eParamDirectOut3].min = 0;
    desc[eParamDirectOut3].max = FR32_MAX;
    desc[eParamDirectOut3].radix = FIX_RADIX;
    
    strcpy(desc[eParamDirectOut4].label, "directout4");
    desc[eParamDirectOut4].type = eParamTypeFix;
    desc[eParamDirectOut4].min = 0;
    desc[eParamDirectOut4].max = FR32_MAX;
    desc[eParamDirectOut4].radix = FIX_RADIX;

    //  Master
    strcpy(desc[eParamMaster].label, "master");
    desc[eParamMaster].type = eParamTypeFix;
    desc[eParamMaster].min = 0;
    desc[eParamMaster].max = FR32_MAX;
    desc[eParamMaster].radix = FIX_RADIX;

    //  Counter
    strcpy(desc[eParamCounter].label, "counter");
    desc[eParamCounter].type = eParamTypeFix;
    desc[eParamCounter].min = 0;
    desc[eParamCounter].max = FR32_MAX;
    desc[eParamCounter].radix = FIX_RADIX;
//*/
}
