#include <string.h>
#include "param_common.h"
#include "params.h" 

void fill_param_desc(ParamDesc *desc) {
    //  Trig
    strcpy(desc[eParamTrig].label, "trig");
    desc[eParamTrig].type = eParamTypeFix;
    desc[eParamTrig].min = 0x00000000;
    desc[eParamTrig].max = FR32_MAX;
    desc[eParamTrig].radix = FIX_RADIX;

    //  Flag
    strcpy(desc[eParamFlag0].label, "flag0");
    desc[eParamFlag0].type = eParamTypeBool;
    desc[eParamFlag0].min = 0;
    desc[eParamFlag0].max = 1;
    desc[eParamFlag0].radix = BOOL_RADIX;
    
    strcpy(desc[eParamFlag1].label, "flag1");
    desc[eParamFlag1].type = eParamTypeBool;
    desc[eParamFlag1].min = 0;
    desc[eParamFlag1].max = 1;
    desc[eParamFlag1].radix = BOOL_RADIX;
    
    strcpy(desc[eParamFlag2].label, "flag2");
    desc[eParamFlag2].type = eParamTypeBool;
    desc[eParamFlag2].min = 0;
    desc[eParamFlag2].max = 1;
    desc[eParamFlag2].radix = BOOL_RADIX;
    
    strcpy(desc[eParamFlag3].label, "flag3");
    desc[eParamFlag3].type = eParamTypeBool;
    desc[eParamFlag3].min = 0;
    desc[eParamFlag3].max = 1;
    desc[eParamFlag3].radix = BOOL_RADIX;

    //  Curve
    strcpy(desc[eParamCurve0].label, "curve0");
    desc[eParamCurve0].type = eParamTypeFix;
    desc[eParamCurve0].min = 0x00000000;
    desc[eParamCurve0].max = FR32_MAX;
    desc[eParamCurve0].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve1].label, "curve1");
    desc[eParamCurve1].type = eParamTypeFix;
    desc[eParamCurve1].min = 0x00000000;
    desc[eParamCurve1].max = FR32_MAX;
    desc[eParamCurve1].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve2].label, "curve2");
    desc[eParamCurve2].type = eParamTypeFix;
    desc[eParamCurve2].min = 0x00000000;
    desc[eParamCurve2].max = FR32_MAX;
    desc[eParamCurve2].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurve3].label, "curve3");
    desc[eParamCurve3].type = eParamTypeFix;
    desc[eParamCurve3].min = 0x00000000;
    desc[eParamCurve3].max = FR32_MAX;
    desc[eParamCurve3].radix = FIX_RADIX;

    //  Time
    strcpy(desc[eParamTime0].label, "time0");
    desc[eParamTime0].type = eParamTypeFix;
    desc[eParamTime0].min = 0x00000000;
    desc[eParamTime0].max = FR32_MAX;
    desc[eParamTime0].radix = FIX_RADIX;
    
    strcpy(desc[eParamTime1].label, "time1");
    desc[eParamTime1].type = eParamTypeFix;
    desc[eParamTime1].min = 0x00000000;
    desc[eParamTime1].max = FR32_MAX;
    desc[eParamTime1].radix = FIX_RADIX;
    
    strcpy(desc[eParamTime2].label, "time2");
    desc[eParamTime2].type = eParamTypeFix;
    desc[eParamTime2].min = 0x00000000;
    desc[eParamTime2].max = FR32_MAX;
    desc[eParamTime2].radix = FIX_RADIX;
    
    strcpy(desc[eParamTime3].label, "time3");
    desc[eParamTime3].type = eParamTypeFix;
    desc[eParamTime3].min = 0x00000000;
    desc[eParamTime3].max = FR32_MAX;
    desc[eParamTime3].radix = FIX_RADIX;
    
    //  paramL
    strcpy(desc[eParamL0].label, "l0");
    desc[eParamL0].type = eParamTypeFix;
    desc[eParamL0].min = 0x00000000;
    desc[eParamL0].max = FR32_MAX;
    desc[eParamL0].radix = FIX_RADIX;
    
    strcpy(desc[eParamL1].label, "l1");
    desc[eParamL1].type = eParamTypeFix;
    desc[eParamL1].min = 0x00000000;
    desc[eParamL1].max = FR32_MAX;
    desc[eParamL1].radix = FIX_RADIX;
    
    strcpy(desc[eParamL2].label, "l2");
    desc[eParamL2].type = eParamTypeFix;
    desc[eParamL2].min = 0x00000000;
    desc[eParamL2].max = FR32_MAX;
    desc[eParamL2].radix = FIX_RADIX;
    
    strcpy(desc[eParamL3].label, "l3");
    desc[eParamL3].type = eParamTypeFix;
    desc[eParamL3].min = 0x00000000;
    desc[eParamL3].max = FR32_MAX;
    desc[eParamL3].radix = FIX_RADIX;

    //  paramP
    strcpy(desc[eParamP0].label, "p0");
    desc[eParamP0].type = eParamTypeFix;
    desc[eParamP0].min = 0x00000000;
    desc[eParamP0].max = FR32_MAX;
    desc[eParamP0].radix = FIX_RADIX;
    
    strcpy(desc[eParamP1].label, "p1");
    desc[eParamP1].type = eParamTypeFix;
    desc[eParamP1].min = 0x00000000;
    desc[eParamP1].max = FR32_MAX;
    desc[eParamP1].radix = FIX_RADIX;
    
    strcpy(desc[eParamP2].label, "p2");
    desc[eParamP2].type = eParamTypeFix;
    desc[eParamP2].min = 0x00000000;
    desc[eParamP2].max = FR32_MAX;
    desc[eParamP2].radix = FIX_RADIX;
    
    strcpy(desc[eParamP3].label, "p3");
    desc[eParamP3].type = eParamTypeFix;
    desc[eParamP3].min = 0x00000000;
    desc[eParamP3].max = FR32_MAX;
    desc[eParamP3].radix = FIX_RADIX;

    //  paramF
    strcpy(desc[eParamF0].label, "f0");
    desc[eParamF0].type = eParamTypeFix;
    desc[eParamF0].min = 0x00000000;
    desc[eParamF0].max = FR32_MAX;
    desc[eParamF0].radix = FIX_RADIX;

    strcpy(desc[eParamF1].label, "f1");
    desc[eParamF1].type = eParamTypeFix;
    desc[eParamF1].min = 0x00000000;
    desc[eParamF1].max = FR32_MAX;
    desc[eParamF1].radix = FIX_RADIX;

    strcpy(desc[eParamF2].label, "f2");
    desc[eParamF2].type = eParamTypeFix;
    desc[eParamF2].min = 0x00000000;
    desc[eParamF2].max = FR32_MAX;
    desc[eParamF2].radix = FIX_RADIX;

    strcpy(desc[eParamF3].label, "f3");
    desc[eParamF3].type = eParamTypeFix;
    desc[eParamF3].min = 0x00000000;
    desc[eParamF3].max = FR32_MAX;
    desc[eParamF3].radix = FIX_RADIX;
    
    //  paramX
    strcpy(desc[eParamX0].label, "x0");
    desc[eParamX0].type = eParamTypeFix;
    desc[eParamX0].min = 0x00000000;
    desc[eParamX0].max = FR32_MAX;
    desc[eParamX0].radix = FIX_RADIX;
    
    strcpy(desc[eParamX1].label, "x1");
    desc[eParamX1].type = eParamTypeFix;
    desc[eParamX1].min = 0x00000000;
    desc[eParamX1].max = FR32_MAX;
    desc[eParamX1].radix = FIX_RADIX;
    
    strcpy(desc[eParamX2].label, "x2");
    desc[eParamX2].type = eParamTypeFix;
    desc[eParamX2].min = 0x00000000;
    desc[eParamX2].max = FR32_MAX;
    desc[eParamX2].radix = FIX_RADIX;
    
    strcpy(desc[eParamX3].label, "x3");
    desc[eParamX3].type = eParamTypeFix;
    desc[eParamX3].min = 0x00000000;
    desc[eParamX3].max = FR32_MAX;
    desc[eParamX3].radix = FIX_RADIX;
    
    strcpy(desc[eParamTest].label, "test");
    desc[eParamTest].type = eParamTypeFix;
    desc[eParamTest].min = 0x00000000;
    desc[eParamTest].max = FR32_MAX;
    desc[eParamTest].radix = FIX_RADIX;
    
    strcpy(desc[eParamCounter].label, "counter");
    desc[eParamCounter].type = eParamTypeFix;
    desc[eParamCounter].min = 0x00000000;
    desc[eParamCounter].max = FR32_MAX;
    desc[eParamCounter].radix = FIX_RADIX;
}
