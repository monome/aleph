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
    
    strcpy(desc[eParamTune0].label, "osc0_tune");
    desc[eParamTune0].type = eParamTypeFix;
    desc[eParamTune0].min = RATIO_MIN;
    desc[eParamTune0].max = RATIO_MAX;
    desc[eParamTune0].radix = RATIO_RADIX;

    strcpy(desc[eParamTune1].label, "osc1_tune");
    desc[eParamTune1].type = eParamTypeFix;
    desc[eParamTune1].min = RATIO_MIN;
    desc[eParamTune1].max = RATIO_MAX;
    desc[eParamTune1].radix = RATIO_RADIX;

    strcpy(desc[eParamTune2].label, "osc2_tune");
    desc[eParamTune2].type = eParamTypeFix;
    desc[eParamTune2].min = RATIO_MIN;
    desc[eParamTune2].max = RATIO_MAX;
    desc[eParamTune2].radix = RATIO_RADIX;

    strcpy(desc[eParamTune3].label, "osc3_tune");
    desc[eParamTune3].type = eParamTypeFix;
    desc[eParamTune3].min = RATIO_MIN;
    desc[eParamTune3].max = RATIO_MAX;
    desc[eParamTune3].radix = RATIO_RADIX;

/*
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

    strcpy(desc[eParamIoAmp0].label, "io0_amp");
    desc[eParamIoAmp0].type = eParamTypeAmp;
    desc[eParamIoAmp0].min = 0;
    desc[eParamIoAmp0].max = FR32_MAX >> 1;
    desc[eParamIoAmp0].radix = 32;

    strcpy(desc[eParamIoAmp1].label, "io1_amp");
    desc[eParamIoAmp1].type = eParamTypeAmp;
    desc[eParamIoAmp1].min = 0;
    desc[eParamIoAmp1].max = FR32_MAX >> 1;
    desc[eParamIoAmp1].radix = 32;

    strcpy(desc[eParamIoAmp2].label, "io2_amp");
    desc[eParamIoAmp2].type = eParamTypeAmp;
    desc[eParamIoAmp2].min = 0;
    desc[eParamIoAmp2].max = FR32_MAX >> 1;
    desc[eParamIoAmp2].radix = 32;

    strcpy(desc[eParamIoAmp3].label, "io3_amp");
    desc[eParamIoAmp3].type = eParamTypeAmp;
    desc[eParamIoAmp3].min = 0;
    desc[eParamIoAmp3].max = FR32_MAX >> 1;
    desc[eParamIoAmp3].radix = 32;

/*
    strcpy(desc[eParamFreq0Slew].label, "osc0_fslew");
    desc[eParamFreq0Slew].type = eParamTypeIntegrator;
    desc[eParamFreq0Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamFreq0Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamFreq0Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamFreq1Slew].label, "osc1_fslew");
    desc[eParamFreq1Slew].type = eParamTypeIntegrator;
    desc[eParamFreq1Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamFreq1Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamFreq1Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamFreq2Slew].label, "osc2_fslew");
    desc[eParamFreq2Slew].type = eParamTypeIntegrator;
    desc[eParamFreq2Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamFreq2Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamFreq2Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamFreq3Slew].label, "osc3_fslew");
    desc[eParamFreq3Slew].type = eParamTypeIntegrator;
    desc[eParamFreq3Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamFreq3Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamFreq3Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamWave0Slew].label, "osc0_wfslew");
    desc[eParamWave0Slew].type = eParamTypeIntegrator;
    desc[eParamWave0Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamWave0Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamWave0Slew].radix = SMOOTH_FREQ_RADIX;
  
    strcpy(desc[eParamWave1Slew].label, "osc1_wfslew");
    desc[eParamWave1Slew].type = eParamTypeIntegrator;
    desc[eParamWave1Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamWave1Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamWave1Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamWave2Slew].label, "osc2_wfslew");
    desc[eParamWave2Slew].type = eParamTypeIntegrator;
    desc[eParamWave2Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamWave2Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamWave2Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamWave3Slew].label, "osc3_wfslew");
    desc[eParamWave3Slew].type = eParamTypeIntegrator;
    desc[eParamWave3Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamWave3Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamWave3Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamAmp0Slew].label, "osc0_ampslew");
    desc[eParamAmp0Slew].type = eParamTypeIntegrator;
    desc[eParamAmp0Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamAmp0Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamAmp0Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamAmp1Slew].label, "osc1_ampslew");
    desc[eParamAmp1Slew].type = eParamTypeIntegrator;
    desc[eParamAmp1Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamAmp1Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamAmp1Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamAmp2Slew].label, "osc2_ampslew");
    desc[eParamAmp2Slew].type = eParamTypeIntegrator;
    desc[eParamAmp2Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamAmp2Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamAmp2Slew].radix = SMOOTH_FREQ_RADIX;

    strcpy(desc[eParamAmp3Slew].label, "osc3_ampslew");
    desc[eParamAmp3Slew].type = eParamTypeIntegrator;
    desc[eParamAmp3Slew].min = SMOOTH_FREQ_MIN;
    desc[eParamAmp3Slew].max = SMOOTH_FREQ_MAX;
    desc[eParamAmp3Slew].radix = SMOOTH_FREQ_RADIX;
*/
}
