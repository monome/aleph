#include <string.h>
#include "param_common.h"
#include "params.h" 

void fill_param_desc(ParamDesc *desc) {
    strcpy(desc[eParamFreq0].label, "osc0_f");
    desc[eParamFreq0].type = eParamTypeNote;
    desc[eParamFreq0].min = OSC_HZ_MIN;
    desc[eParamFreq0].max = OSC_HZ_MAX;
    desc[eParamFreq0].radix = OSC_HZ_RADIX;
    //radix: 1 = signed fract (1.32), 16 = 16.16, 32 = signed int

    strcpy(desc[eParamFreq1].label, "osc1_f");
    desc[eParamFreq1].type = eParamTypeNote;
    desc[eParamFreq1].min = OSC_HZ_MIN;
    desc[eParamFreq1].max = OSC_HZ_MAX;
    desc[eParamFreq1].radix = OSC_HZ_RADIX;

    strcpy(desc[eParamFreq2].label, "osc2_f");
    desc[eParamFreq2].type = eParamTypeNote;
    desc[eParamFreq2].min = OSC_HZ_MIN;
    desc[eParamFreq2].max = OSC_HZ_MAX;
    desc[eParamFreq2].radix = OSC_HZ_RADIX;

    strcpy(desc[eParamFreq3].label, "osc3_f");
    desc[eParamFreq3].type = eParamTypeNote;
    desc[eParamFreq3].min = OSC_HZ_MIN;
    desc[eParamFreq3].max = OSC_HZ_MAX;
    desc[eParamFreq3].radix = OSC_HZ_RADIX;

    strcpy(desc[eParamFreqFine0].label, "osc0_ff");
    desc[eParamFreqFine0].type = eParamTypeNote;
    desc[eParamFreqFine0].min = 0x00000000;
    desc[eParamFreqFine0].max = CV_VOCT_MAX;
    desc[eParamFreqFine0].radix = CV_VOCT_RADIX;
    
    strcpy(desc[eParamFreqFine1].label, "osc1_ff");
    desc[eParamFreqFine1].type = eParamTypeNote;
    desc[eParamFreqFine1].min = 0x00000000;
    desc[eParamFreqFine1].max = CV_VOCT_MAX;
    desc[eParamFreqFine1].radix = CV_VOCT_RADIX;

    strcpy(desc[eParamFreqFine2].label, "osc2_ff");
    desc[eParamFreqFine2].type = eParamTypeNote;
    desc[eParamFreqFine2].min = 0x00000000;
    desc[eParamFreqFine2].max = CV_VOCT_MAX;
    desc[eParamFreqFine2].radix = CV_VOCT_RADIX;

    strcpy(desc[eParamFreqFine3].label, "osc3_ff");
    desc[eParamFreqFine3].type = eParamTypeNote;
    desc[eParamFreqFine3].min = 0x00000000;
    desc[eParamFreqFine3].max = CV_VOCT_MAX;
    desc[eParamFreqFine3].radix = CV_VOCT_RADIX;
    
    strcpy(desc[eParamFFAmount0].label, "osc0_ffamount");
    desc[eParamFFAmount0].type = eParamTypeFix;
    desc[eParamFFAmount0].min = 0;
    desc[eParamFFAmount0].max = FR32_MAX;
    desc[eParamFFAmount0].radix = 1;

    strcpy(desc[eParamFFAmount1].label, "osc1_ffamount");
    desc[eParamFFAmount1].type = eParamTypeFix;
    desc[eParamFFAmount1].min = 0; 
    desc[eParamFFAmount1].max = FR32_MAX;
    desc[eParamFFAmount1].radix = 1;

    strcpy(desc[eParamFFAmount2].label, "osc2_ffamount");
    desc[eParamFFAmount2].type = eParamTypeFix;
    desc[eParamFFAmount2].min = 0;
    desc[eParamFFAmount2].max = FR32_MAX;
    desc[eParamFFAmount2].radix = 1;

    strcpy(desc[eParamFFAmount3].label, "osc3_ffamount");
    desc[eParamFFAmount3].type = eParamTypeFix;
    desc[eParamFFAmount3].min = 0;
    desc[eParamFFAmount3].max = FR32_MAX;
    desc[eParamFFAmount3].radix = 1;

    strcpy(desc[eParamWave0].label, "osc0_wf");
    desc[eParamWave0].type = eParamTypeFix;
    desc[eParamWave0].min = 0;
    desc[eParamWave0].max = FR32_MAX;
    desc[eParamWave0].radix = 1;

    strcpy(desc[eParamWave1].label, "osc1_wf");
    desc[eParamWave1].type = eParamTypeFix;
    desc[eParamWave1].min = 0;
    desc[eParamWave1].max = FR32_MAX;
    desc[eParamWave1].radix = 1;

    strcpy(desc[eParamWave2].label, "osc2_wf");
    desc[eParamWave2].type = eParamTypeFix;
    desc[eParamWave2].min = 0;
    desc[eParamWave2].max = FR32_MAX;
    desc[eParamWave2].radix = 1;

    strcpy(desc[eParamWave3].label, "osc3_wf");
    desc[eParamWave3].type = eParamTypeFix;
    desc[eParamWave3].min = 0;
    desc[eParamWave3].max = FR32_MAX;
    desc[eParamWave3].radix = 1;

    strcpy(desc[eParamSyncTrig].label, "sync_trig");
    desc[eParamSyncTrig].type = eParamTypeBool;
    desc[eParamSyncTrig].min = 0;
    desc[eParamSyncTrig].max = 1;
    desc[eParamSyncTrig].radix = 2;
    
    strcpy(desc[eParamTripPoint0].label, "osc0_trip");
    desc[eParamTripPoint0].type = eParamTypeFix;
    desc[eParamTripPoint0].min = 0;
    desc[eParamTripPoint0].max = FR32_MAX;
    desc[eParamTripPoint0].radix = 1;

    strcpy(desc[eParamTripPoint1].label, "osc1_trip");
    desc[eParamTripPoint1].type = eParamTypeFix;
    desc[eParamTripPoint1].min = 0;
    desc[eParamTripPoint1].max = FR32_MAX;
    desc[eParamTripPoint1].radix = 1;

    strcpy(desc[eParamTripPoint2].label, "osc2_trip");
    desc[eParamTripPoint2].type = eParamTypeFix;
    desc[eParamTripPoint2].min = 0;
    desc[eParamTripPoint2].max = FR32_MAX;
    desc[eParamTripPoint2].radix = 1;

    strcpy(desc[eParamTripPoint3].label, "osc3_trip");
    desc[eParamTripPoint3].type = eParamTypeFix;
    desc[eParamTripPoint3].min = 0;
    desc[eParamTripPoint3].max = FR32_MAX;
    desc[eParamTripPoint3].radix = 1;

    strcpy(desc[eParamAmp0].label, "osc0_amp");
    desc[eParamAmp0].type = eParamTypeAmp;
    desc[eParamAmp0].min = 0;
    desc[eParamAmp0].max = FR32_MAX >> 1;
    desc[eParamAmp0].radix = 32;

    strcpy(desc[eParamAmp1].label, "osc1_amp");
    desc[eParamAmp1].type = eParamTypeAmp;
    desc[eParamAmp1].min = 0;
    desc[eParamAmp1].max = FR32_MAX >> 1;
    desc[eParamAmp1].radix = 32;

    strcpy(desc[eParamAmp2].label, "osc2_amp");
    desc[eParamAmp2].type = eParamTypeAmp;
    desc[eParamAmp2].min = 0;
    desc[eParamAmp2].max = FR32_MAX >> 1;
    desc[eParamAmp2].radix = 32;

    strcpy(desc[eParamAmp3].label, "osc3_amp");
    desc[eParamAmp3].type = eParamTypeAmp;
    desc[eParamAmp3].min = 0;
    desc[eParamAmp3].max = FR32_MAX >> 1;
    desc[eParamAmp3].radix = 32;
}
