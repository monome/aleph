#include <string.h>
#include "param_common.h"
#include "params.h" 

void fill_param_desc(ParamDesc *desc) {
    strcpy(desc[eParamFree0].label, "cv0_f");
    desc[eParamFree0].type = eParamTypeFix;
    desc[eParamFree0].min = 0x00000000;
    desc[eParamFree0].max = FR32_MAX;
    desc[eParamFree0].radix = FIX_RADIX;

    strcpy(desc[eParamFree1].label, "cv1_f");
    desc[eParamFree1].type = eParamTypeFix;
    desc[eParamFree1].min = 0x00000000;
    desc[eParamFree1].max = FR32_MAX;
    desc[eParamFree1].radix = FIX_RADIX;

    strcpy(desc[eParamFree2].label, "cv2_f");
    desc[eParamFree2].type = eParamTypeFix;
    desc[eParamFree2].min = 0x00000000;
    desc[eParamFree2].max = FR32_MAX;
    desc[eParamFree2].radix = FIX_RADIX;

    strcpy(desc[eParamFree3].label, "cv3_f");
    desc[eParamFree3].type = eParamTypeFix;
    desc[eParamFree3].min = 0x00000000;
    desc[eParamFree3].max = FR32_MAX;
    desc[eParamFree3].radix = FIX_RADIX;
    
    strcpy(desc[eParamTransposed0].label, "cv0_t");
    desc[eParamTransposed0].type = eParamTypeFix;
    desc[eParamTransposed0].min = 0x2000;
    desc[eParamTransposed0].max = 0x20000;
    desc[eParamTransposed0].radix = FIX_RADIX;

    strcpy(desc[eParamTransposed1].label, "cv1_t");
    desc[eParamTransposed1].type = eParamTypeFix;
    desc[eParamTransposed1].min = 0x2000;
    desc[eParamTransposed1].max = 0x20000;
    desc[eParamTransposed1].radix = FIX_RADIX;

    strcpy(desc[eParamTransposed2].label, "cv2_t");
    desc[eParamTransposed2].type = eParamTypeFix;
    desc[eParamTransposed2].min = 0x2000;
    desc[eParamTransposed2].max = 0x20000;
    desc[eParamTransposed2].radix = FIX_RADIX;

    strcpy(desc[eParamTransposed3].label, "cv3_t");
    desc[eParamTransposed3].type = eParamTypeFix;
    desc[eParamTransposed3].min = 0x2000;
    desc[eParamTransposed3].max = 0x20000;
    desc[eParamTransposed3].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurveTime0].label, "curve0_time");
    desc[eParamCurveTime0].type = eParamTypeFix;
    desc[eParamCurveTime0].min = 0x00000000;
    desc[eParamCurveTime0].max = FR32_MAX;
    desc[eParamCurveTime0].radix = FIX_RADIX;

    strcpy(desc[eParamCurveTime1].label, "curve1_time");
    desc[eParamCurveTime1].type = eParamTypeFix;
    desc[eParamCurveTime1].min = 0x00000000;
    desc[eParamCurveTime1].max = FR32_MAX;
    desc[eParamCurveTime1].radix = FIX_RADIX;

    strcpy(desc[eParamCurveTime2].label, "curve2_time");
    desc[eParamCurveTime2].type = eParamTypeFix;
    desc[eParamCurveTime2].min = 0x00000000;
    desc[eParamCurveTime2].max = FR32_MAX;
    desc[eParamCurveTime2].radix = FIX_RADIX;

    strcpy(desc[eParamCurveTime3].label, "curve3_time");
    desc[eParamCurveTime3].type = eParamTypeFix;
    desc[eParamCurveTime3].min = 0x00000000;
    desc[eParamCurveTime3].max = FR32_MAX;
    desc[eParamCurveTime3].radix = FIX_RADIX;
    
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

    strcpy(desc[eParamCurveDest0].label, "curve0_dest");
    desc[eParamCurveDest0].type = eParamTypeFix;
    desc[eParamCurveDest0].min = 0x00000000;
    desc[eParamCurveDest0].max = FR32_MAX;
    desc[eParamCurveDest0].radix = FIX_RADIX;

    strcpy(desc[eParamCurveDest1].label, "curve1_dest");
    desc[eParamCurveDest1].type = eParamTypeFix;
    desc[eParamCurveDest1].min = 0x00000000;
    desc[eParamCurveDest1].max = FR32_MAX;
    desc[eParamCurveDest1].radix = FIX_RADIX;

    strcpy(desc[eParamCurveDest2].label, "curve2_dest");
    desc[eParamCurveDest2].type = eParamTypeFix;
    desc[eParamCurveDest2].min = 0x00000000;
    desc[eParamCurveDest2].max = FR32_MAX;
    desc[eParamCurveDest2].radix = FIX_RADIX;

    strcpy(desc[eParamCurveDest3].label, "curve3_dest");
    desc[eParamCurveDest3].type = eParamTypeFix;
    desc[eParamCurveDest3].min = 0x00000000;
    desc[eParamCurveDest3].max = FR32_MAX;
    desc[eParamCurveDest3].radix = FIX_RADIX;
    
    strcpy(desc[eParamCurveTrig0].label, "curve0_trig");
    desc[eParamCurveTrig0].type = eParamTypeBool;
    desc[eParamCurveTrig0].min = 0;
    desc[eParamCurveTrig0].max = 1;
    desc[eParamCurveTrig0].radix = BOOL_RADIX;

    strcpy(desc[eParamCurveTrig1].label, "curve1_trig");
    desc[eParamCurveTrig1].type = eParamTypeBool;
    desc[eParamCurveTrig1].min = 0;
    desc[eParamCurveTrig1].max = 1;
    desc[eParamCurveTrig1].radix = BOOL_RADIX;

    strcpy(desc[eParamCurveTrig2].label, "curve2_trig");
    desc[eParamCurveTrig2].type = eParamTypeBool;
    desc[eParamCurveTrig2].min = 0;
    desc[eParamCurveTrig2].max = 1;
    desc[eParamCurveTrig2].radix = BOOL_RADIX;

    strcpy(desc[eParamCurveTrig3].label, "curve3_trig");
    desc[eParamCurveTrig3].type = eParamTypeBool;
    desc[eParamCurveTrig3].min = 0;
    desc[eParamCurveTrig3].max = 1;
    desc[eParamCurveTrig3].radix = BOOL_RADIX;
}
