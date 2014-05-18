#include <string.h>
#include "param_common.h"
#include "params.h" 

void fill_param_desc(ParamDesc *desc) {
    strcpy(desc[eParamFreq0].label, "osc0_f");
    desc[eParamFreq0].type = eParamTypeNote;
    desc[eParamFreq0].min = OSC_HZ_MIN;
    desc[eParamFreq0].max = OSC_HZ_MAX;
    desc[eParamFreq0].radix = OSC_HZ_RADIX;

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

//    strcpy(desc[eParamIdxReset].label, "sync_trig");
//    desc[eParamIdxReset].type = eParamTypeBool;
//    desc[eParamIdxReset].min = 0;
//    desc[eParamIdxReset].max = 1;
//    desc[eParamIdxReset].radix = 2;

/*
    strcpy(desc[eParamIdxMod0].label, "osc0_pm");
    desc[eParamAmp0].type = eParamTypeAmp;
    desc[eParamAmp0].min = 0;
    desc[eParamAmp0].max = FR32_MAX >> 1;
    desc[eParamAmp0].radix = 32;
    
    strcpy(desc[eParamIdxMod1].label, "osc1_pm");
    desc[eParamAmp0].type = eParamTypeAmp;
    desc[eParamAmp0].min = 0;
    desc[eParamAmp0].max = FR32_MAX >> 1;
    desc[eParamAmp0].radix = 32;
    
    strcpy(desc[eParamIdxMod2].label, "osc2_pm");
    desc[eParamAmp0].type = eParamTypeAmp;
    desc[eParamAmp0].min = 0;
    desc[eParamAmp0].max = FR32_MAX >> 1;
    desc[eParamAmp0].radix = 32;
    
    strcpy(desc[eParamidxMod3].label, "osc3_pm");
    desc[eParamAmp0].type = eParamTypeAmp;
    desc[eParamAmp0].min = 0;
    desc[eParamAmp0].max = FR32_MAX >> 1;
    desc[eParamAmp0].radix = 32;
*/
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
