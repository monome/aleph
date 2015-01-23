
#include <string.h>
#include "param_common.h"
#include "params.h" 

void fill_param_desc(ParamDesc *desc) {
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

    //  Time
    strcpy(desc[eParamTime0].label, "time0");
    desc[eParamTime0].type = eParamTypeFix;
    desc[eParamTime0].min = 0;
    desc[eParamTime0].max = FR32_MAX;
    desc[eParamTime0].radix = FIX_RADIX;
    
    strcpy(desc[eParamTime1].label, "time1");
    desc[eParamTime1].type = eParamTypeFix;
    desc[eParamTime1].min = 0;
    desc[eParamTime1].max = FR32_MAX;
    desc[eParamTime1].radix = FIX_RADIX;
    
    strcpy(desc[eParamTime2].label, "time2");
    desc[eParamTime2].type = eParamTypeFix;
    desc[eParamTime2].min = 0;
    desc[eParamTime2].max = FR32_MAX;
    desc[eParamTime2].radix = FIX_RADIX;
    
    strcpy(desc[eParamTime3].label, "time3");
    desc[eParamTime3].type = eParamTypeFix;
    desc[eParamTime3].min = 0;
    desc[eParamTime3].max = FR32_MAX;
    desc[eParamTime3].radix = FIX_RADIX;
    
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
    
    //  Position
    strcpy(desc[eParamPosition0].label, "position0");
    desc[eParamPosition0].type = eParamTypeFix;
    desc[eParamPosition0].min = 0;
    desc[eParamPosition0].max = FR32_MAX;
    desc[eParamPosition0].radix = FIX_RADIX;

    strcpy(desc[eParamPosition1].label, "position1");
    desc[eParamPosition1].type = eParamTypeFix;
    desc[eParamPosition1].min = 0;
    desc[eParamPosition1].max = FR32_MAX;
    desc[eParamPosition1].radix = FIX_RADIX;

    strcpy(desc[eParamPosition2].label, "position2");
    desc[eParamPosition2].type = eParamTypeFix;
    desc[eParamPosition2].min = 0;
    desc[eParamPosition2].max = FR32_MAX;
    desc[eParamPosition2].radix = FIX_RADIX;

    strcpy(desc[eParamPosition3].label, "position3");
    desc[eParamPosition3].type = eParamTypeFix;
    desc[eParamPosition3].min = 0;
    desc[eParamPosition3].max = FR32_MAX;
    desc[eParamPosition3].radix = FIX_RADIX;
    
    //  Loop
    strcpy(desc[eParamLoop0].label, "loop0");
    desc[eParamLoop0].type = eParamTypeFix;
    desc[eParamLoop0].min = 0;
    desc[eParamLoop0].max = FR32_MAX;
    desc[eParamLoop0].radix = FIX_RADIX;
    
    strcpy(desc[eParamLoop1].label, "loop1");
    desc[eParamLoop1].type = eParamTypeFix;
    desc[eParamLoop1].min = 0;
    desc[eParamLoop1].max = FR32_MAX;
    desc[eParamLoop1].radix = FIX_RADIX;
    
    strcpy(desc[eParamLoop2].label, "loop2");
    desc[eParamLoop2].type = eParamTypeFix;
    desc[eParamLoop2].min = 0;
    desc[eParamLoop2].max = FR32_MAX;
    desc[eParamLoop2].radix = FIX_RADIX;
    
    strcpy(desc[eParamLoop3].label, "loop3");
    desc[eParamLoop3].type = eParamTypeFix;
    desc[eParamLoop3].min = 0;
    desc[eParamLoop3].max = FR32_MAX;
    desc[eParamLoop3].radix = FIX_RADIX;

    //  Level
    strcpy(desc[eParamLevel0].label, "level0");
    desc[eParamLevel0].type = eParamTypeFix;
    desc[eParamLevel0].min = 0;
    desc[eParamLevel0].max = FR32_MAX;
    desc[eParamLevel0].radix = FIX_RADIX;

    strcpy(desc[eParamLevel1].label, "level1");
    desc[eParamLevel1].type = eParamTypeFix;
    desc[eParamLevel1].min = 0;
    desc[eParamLevel1].max = FR32_MAX;
    desc[eParamLevel1].radix = FIX_RADIX;

    strcpy(desc[eParamLevel2].label, "level2");
    desc[eParamLevel2].type = eParamTypeFix;
    desc[eParamLevel2].min = 0;
    desc[eParamLevel2].max = FR32_MAX;
    desc[eParamLevel2].radix = FIX_RADIX;

    strcpy(desc[eParamLevel3].label, "level3");
    desc[eParamLevel3].type = eParamTypeFix;
    desc[eParamLevel3].min = 0;
    desc[eParamLevel3].max = FR32_MAX;
    desc[eParamLevel3].radix = FIX_RADIX;

    //  Frequency
    strcpy(desc[eParamFrequency0].label, "frequency0");
    desc[eParamFrequency0].type = eParamTypeFix;
    desc[eParamFrequency0].min = 0;
    desc[eParamFrequency0].max = FR32_MAX;
    desc[eParamFrequency0].radix = FIX_RADIX;

    strcpy(desc[eParamFrequency1].label, "frequency1");
    desc[eParamFrequency1].type = eParamTypeFix;
    desc[eParamFrequency1].min = 0;
    desc[eParamFrequency1].max = FR32_MAX;
    desc[eParamFrequency1].radix = FIX_RADIX;

    strcpy(desc[eParamFrequency2].label, "frequency2");
    desc[eParamFrequency2].type = eParamTypeFix;
    desc[eParamFrequency2].min = 0;
    desc[eParamFrequency2].max = FR32_MAX;
    desc[eParamFrequency2].radix = FIX_RADIX;

    strcpy(desc[eParamFrequency3].label, "frequency3");
    desc[eParamFrequency3].type = eParamTypeFix;
    desc[eParamFrequency3].min = 0;
    desc[eParamFrequency3].max = FR32_MAX;
    desc[eParamFrequency3].radix = FIX_RADIX;

    //  Frames
    strcpy(desc[eParamFrames].label, "frames");
    desc[eParamFrames].type = eParamTypeFix;
    desc[eParamFrames].min = 0;
    desc[eParamFrames].max = FR32_MAX;
    desc[eParamFrames].radix = FIX_RADIX;

    //  Buffer Length
    strcpy(desc[eParamBufferLength0].label, "length0");
    desc[eParamBufferLength0].type = eParamTypeFix;
    desc[eParamBufferLength0].min = 0;
    desc[eParamBufferLength0].max = FR32_MAX;
    desc[eParamBufferLength0].radix = FIX_RADIX;

    strcpy(desc[eParamBufferLength1].label, "length1");
    desc[eParamBufferLength1].type = eParamTypeFix;
    desc[eParamBufferLength1].min = 0;
    desc[eParamBufferLength1].max = FR32_MAX;
    desc[eParamBufferLength1].radix = FIX_RADIX;

    strcpy(desc[eParamBufferLength2].label, "length2");
    desc[eParamBufferLength2].type = eParamTypeFix;
    desc[eParamBufferLength2].min = 0;
    desc[eParamBufferLength2].max = FR32_MAX;
    desc[eParamBufferLength2].radix = FIX_RADIX;

    strcpy(desc[eParamBufferLength3].label, "length3");
    desc[eParamBufferLength3].type = eParamTypeFix;
    desc[eParamBufferLength3].min = 0;
    desc[eParamBufferLength3].max = FR32_MAX;
    desc[eParamBufferLength3].radix = FIX_RADIX;

    //  Toggle Buffer
    strcpy(desc[eParamToggleBuf].label, "buffer");
    desc[eParamToggleBuf].type = eParamTypeFix;
    desc[eParamToggleBuf].min = 0;
    desc[eParamToggleBuf].max = FR32_MAX;
    desc[eParamToggleBuf].radix = FIX_RADIX;

    //  Toggle Recording State
    strcpy(desc[eParamToggleRec].label, "record");
    desc[eParamToggleRec].type = eParamTypeFix;
    desc[eParamToggleRec].min = 0;
    desc[eParamToggleRec].max = FR32_MAX;
    desc[eParamToggleRec].radix = FIX_RADIX;

    //  Counter
    strcpy(desc[eParamSqCounter].label, "counter");
    desc[eParamSqCounter].type = eParamTypeFix;
    desc[eParamSqCounter].min = 0;
    desc[eParamSqCounter].max = FR32_MAX;
    desc[eParamSqCounter].radix = FIX_RADIX;
    
    //  Length
    strcpy(desc[eParamSqLength].label, "length");
    desc[eParamSqLength].type = eParamTypeFix;
    desc[eParamSqLength].min = 0;
    desc[eParamSqLength].max = FR32_MAX;
    desc[eParamSqLength].radix = FIX_RADIX;

    //  Frame Forward
    strcpy(desc[eParamFrameFwd].label, "fwd");
    desc[eParamFrameFwd].type = eParamTypeFix;
    desc[eParamFrameFwd].min = 0;
    desc[eParamFrameFwd].max = FR32_MAX;
    desc[eParamFrameFwd].radix = FIX_RADIX;

    //  Frame Backward
    strcpy(desc[eParamFrameBwd].label, "bwd");
    desc[eParamFrameBwd].type = eParamTypeFix;
    desc[eParamFrameBwd].min = 0;
    desc[eParamFrameBwd].max = FR32_MAX;
    desc[eParamFrameBwd].radix = FIX_RADIX;

    //  Toggle Motor State
    strcpy(desc[eParamToggleMotor].label, "motor");
    desc[eParamToggleMotor].type = eParamTypeFix;
    desc[eParamToggleMotor].min = 0;
    desc[eParamToggleMotor].max = FR32_MAX;
    desc[eParamToggleMotor].radix = FIX_RADIX;
}
