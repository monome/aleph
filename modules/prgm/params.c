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

    strcpy(desc[eParamTranspose0].label, "osc0_t");
    desc[eParamTranspose0].type = eParamTypeNote;
    desc[eParamTranspose0].min = 0;
    desc[eParamTranspose0].max = OSC_HZ_MAX;
    desc[eParamTranspose0].radix = OSC_HZ_RADIX;
    
    strcpy(desc[eParamTranspose1].label, "osc1_t");
    desc[eParamTranspose1].type = eParamTypeNote;
    desc[eParamTranspose1].min = 0;
    desc[eParamTranspose1].max = OSC_HZ_MAX;
    desc[eParamTranspose1].radix = OSC_HZ_RADIX;

    strcpy(desc[eParamTranspose2].label, "osc2_t");
    desc[eParamTranspose2].type = eParamTypeNote;
    desc[eParamTranspose2].min = 0;
    desc[eParamTranspose2].max = OSC_HZ_MAX;
    desc[eParamTranspose2].radix = OSC_HZ_RADIX;

    strcpy(desc[eParamTranspose3].label, "osc3_t");
    desc[eParamTranspose3].type = eParamTypeNote;
    desc[eParamTranspose3].min = 0;
    desc[eParamTranspose3].max = OSC_HZ_MAX;
    desc[eParamTranspose3].radix = OSC_HZ_RADIX;
    
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

    strcpy(desc[eParamTab0].label, "osc0_tab");
    desc[eParamTab0].type = eParamTypeFix;
    desc[eParamTab0].min = 0;
    desc[eParamTab0].max = 2;
    desc[eParamTab0].radix = 0;

    strcpy(desc[eParamTab1].label, "osc1_tab");
    desc[eParamTab1].type = eParamTypeFix;
    desc[eParamTab1].min = 0;
    desc[eParamTab1].max = 2;
    desc[eParamTab1].radix = 0;

    strcpy(desc[eParamTab2].label, "osc2_tab");
    desc[eParamTab2].type = eParamTypeFix;
    desc[eParamTab2].min = 0;
    desc[eParamTab2].max = 2;
    desc[eParamTab2].radix = 0;

    strcpy(desc[eParamTab3].label, "osc3_tab");
    desc[eParamTab3].type = eParamTypeFix;
    desc[eParamTab3].min = 0;
    desc[eParamTab3].max = 2;
    desc[eParamTab3].radix = 0;
    
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

    strcpy(desc[eParamCVfreq0].label, "cv0_f");
    desc[eParamCVfreq0].type = eParamTypeFix;
    desc[eParamCVfreq0].min = 0x00000000;
    desc[eParamCVfreq0].max = PARAM_DAC_MAX;
    desc[eParamCVfreq0].radix = PARAM_DAC_RADIX;

    strcpy(desc[eParamCVfreq1].label, "cv1_f");
    desc[eParamCVfreq1].type = eParamTypeFix;
    desc[eParamCVfreq1].min = 0x00000000;
    desc[eParamCVfreq1].max = PARAM_DAC_MAX;
    desc[eParamCVfreq1].radix = PARAM_DAC_RADIX;

    strcpy(desc[eParamCVfreq2].label, "cv2_f");
    desc[eParamCVfreq2].type = eParamTypeFix;
    desc[eParamCVfreq2].min = 0x00000000;
    desc[eParamCVfreq2].max = PARAM_DAC_MAX;
    desc[eParamCVfreq2].radix = PARAM_DAC_RADIX;

    strcpy(desc[eParamCVfreq3].label, "cv3_f");
    desc[eParamCVfreq3].type = eParamTypeFix;
    desc[eParamCVfreq3].min = 0x00000000;
    desc[eParamCVfreq3].max = PARAM_DAC_MAX;
    desc[eParamCVfreq3].radix = PARAM_DAC_RADIX;
    
    strcpy(desc[eParamCVtranspose0].label, "cv0_t");
    desc[eParamCVtranspose0].type = eParamTypeFix;
    desc[eParamCVtranspose0].min = 0x00000000;
    desc[eParamCVtranspose0].max = PARAM_DAC_MAX;
    desc[eParamCVtranspose0].radix = PARAM_DAC_RADIX;

    strcpy(desc[eParamCVtranspose1].label, "cv1_t");
    desc[eParamCVtranspose1].type = eParamTypeFix;
    desc[eParamCVtranspose1].min = 0x00000000;
    desc[eParamCVtranspose1].max = PARAM_DAC_MAX;
    desc[eParamCVtranspose1].radix = PARAM_DAC_RADIX;

    strcpy(desc[eParamCVtranspose2].label, "cv2_t");
    desc[eParamCVtranspose2].type = eParamTypeFix;
    desc[eParamCVtranspose2].min = 0x00000000;
    desc[eParamCVtranspose2].max = PARAM_DAC_MAX;
    desc[eParamCVtranspose2].radix = PARAM_DAC_RADIX;

    strcpy(desc[eParamCVtranspose3].label, "cv3_t");
    desc[eParamCVtranspose3].type = eParamTypeFix;
    desc[eParamCVtranspose3].min = 0x00000000;
    desc[eParamCVtranspose3].max = PARAM_DAC_MAX;
    desc[eParamCVtranspose3].radix = PARAM_DAC_RADIX;
}
