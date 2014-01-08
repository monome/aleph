#include <string.h>

#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "module_custom.h"
#include "osc.h"
#include "params.h" 

void fill_param_desc(void) {
  strcpy(gModuleData->paramDesc[eParamFreq1].label, "hz1");
  gModuleData->paramDesc[eParamFreq1].type = eParamTypeNote;
  gModuleData->paramDesc[eParamFreq1].min = OSC_HZ_MIN;
  gModuleData->paramDesc[eParamFreq1].max = OSC_HZ_MAX;
  gModuleData->paramDesc[eParamFreq1].radix = OSC_HZ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamFreq2].label, "hz2");
  gModuleData->paramDesc[eParamFreq2].type = eParamTypeNote;
  gModuleData->paramDesc[eParamFreq2].min = OSC_HZ_MIN;
  gModuleData->paramDesc[eParamFreq2].max = OSC_HZ_MAX;
  gModuleData->paramDesc[eParamFreq2].radix = OSC_HZ_RADIX;

  strcpy(gModuleData->paramDesc[eParamTune1].label, "tune1");
  gModuleData->paramDesc[eParamTune1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamTune1].min = RATIO_MIN;
  gModuleData->paramDesc[eParamTune1].max = RATIO_MAX;
  gModuleData->paramDesc[eParamTune1].radix = RATIO_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamTune2].label, "tune2");
  gModuleData->paramDesc[eParamTune2].type = eParamTypeFix;
  gModuleData->paramDesc[eParamTune2].min = RATIO_MIN;
  gModuleData->paramDesc[eParamTune2].max = RATIO_MAX;
  gModuleData->paramDesc[eParamTune2].radix = RATIO_RADIX;
    
  strcpy(gModuleData->paramDesc[eParamAmp1].label, "amp1");
  gModuleData->paramDesc[eParamAmp1].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamAmp1].min = 0;
  gModuleData->paramDesc[eParamAmp1].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamAmp1].radix = 32;
  
  strcpy(gModuleData->paramDesc[eParamAmp2].label, "amp2");
  gModuleData->paramDesc[eParamAmp2].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamAmp2].min = 0;
  gModuleData->paramDesc[eParamAmp2].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamAmp2].radix = 32;
  
  strcpy(gModuleData->paramDesc[eParamIoAmp0].label, "ioAmp1");
  gModuleData->paramDesc[eParamIoAmp0].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp0].min = 0;
  gModuleData->paramDesc[eParamIoAmp0].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp0].radix = 32;

  strcpy(gModuleData->paramDesc[eParamIoAmp1].label, "ioAmp2");
  gModuleData->paramDesc[eParamIoAmp1].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp1].min = 0;
  gModuleData->paramDesc[eParamIoAmp1].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp1].radix = 32;

  strcpy(gModuleData->paramDesc[eParamIoAmp2].label, "ioAmp3");
  gModuleData->paramDesc[eParamIoAmp2].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp2].min = 0;
  gModuleData->paramDesc[eParamIoAmp2].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp2].radix = 32;

  strcpy(gModuleData->paramDesc[eParamIoAmp3].label, "ioAmp4");
  gModuleData->paramDesc[eParamIoAmp3].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp3].min = 0;
  gModuleData->paramDesc[eParamIoAmp3].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp3].radix = 32;

  strcpy(gModuleData->paramDesc[eParamPm12].label, "pm12");
  gModuleData->paramDesc[eParamPm12].type = eParamTypeFix;
  gModuleData->paramDesc[eParamPm12].min = 0;
  gModuleData->paramDesc[eParamPm12].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamPm12].radix = 1;

  strcpy(gModuleData->paramDesc[eParamPm21].label, "pm21");
  gModuleData->paramDesc[eParamPm21].type = eParamTypeFix;
  gModuleData->paramDesc[eParamPm21].min = 0;
  gModuleData->paramDesc[eParamPm21].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamPm21].radix = 1;

  strcpy(gModuleData->paramDesc[eParamWm12].label, "wm12");
  gModuleData->paramDesc[eParamWm12].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWm12].min = 0;
  gModuleData->paramDesc[eParamWm12].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWm12].radix = 1;

  strcpy(gModuleData->paramDesc[eParamWm21].label, "wm21");
  gModuleData->paramDesc[eParamWm21].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWm21].min = 0;
  gModuleData->paramDesc[eParamWm21].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWm21].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamWave1].label, "wave1");
  gModuleData->paramDesc[eParamWave1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWave1].min = 0;
  gModuleData->paramDesc[eParamWave1].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWave1].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamWave2].label, "wave2");
  gModuleData->paramDesc[eParamWave2].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWave2].min = 0;
  gModuleData->paramDesc[eParamWave2].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWave2].radix = 1;
  

  strcpy(gModuleData->paramDesc[eParamBl1].label, "bl1");
  gModuleData->paramDesc[eParamBl1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamBl1].min = 0;
  gModuleData->paramDesc[eParamBl1].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamBl1].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamBl2].label, "bl2");
  gModuleData->paramDesc[eParamBl2].type = eParamTypeFix;
  gModuleData->paramDesc[eParamBl2].min = 0;
  gModuleData->paramDesc[eParamBl2].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamBl2].radix = 1;

  strcpy(gModuleData->paramDesc[eParamFreq1Smooth].label, "hz1Slew");
  gModuleData->paramDesc[eParamFreq1Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamFreq1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq1Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamFreq1Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamFreq2Smooth].label, "hz2Slew");
  gModuleData->paramDesc[eParamFreq2Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamFreq2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq2Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamFreq2Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamPm12Smooth].label, "pm12Slew");
  gModuleData->paramDesc[eParamPm12Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamPm12Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamPm12Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamPm12Smooth].radix = SMOOTH_FREQ_RADIX;

  strcpy(gModuleData->paramDesc[eParamPm21Smooth].label, "pm21Slew");
  gModuleData->paramDesc[eParamPm21Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamPm21Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamPm21Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamPm21Smooth].radix = SMOOTH_FREQ_RADIX;

  strcpy(gModuleData->paramDesc[eParamWm12Smooth].label, "wm12Slew");
  gModuleData->paramDesc[eParamWm12Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWm12Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWm12Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamWm12Smooth].radix = SMOOTH_FREQ_RADIX;

  strcpy(gModuleData->paramDesc[eParamWm21Smooth].label, "wm21Slew");
  gModuleData->paramDesc[eParamWm21Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWm21Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWm21Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamWm21Smooth].radix = SMOOTH_FREQ_RADIX;

  strcpy(gModuleData->paramDesc[eParamWave1Smooth].label, "wave1Slew");
  gModuleData->paramDesc[eParamWave1Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWave1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWave1Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamWave1Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamWave2Smooth].label, "wave2Slew");
  gModuleData->paramDesc[eParamWave2Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWave2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWave2Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamWave2Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamAmp1Smooth].label, "amp1Slew");
  gModuleData->paramDesc[eParamAmp1Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamAmp1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamAmp1Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamAmp1Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamAmp2Smooth].label, "amp2Slew");
  gModuleData->paramDesc[eParamAmp2Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamAmp2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamAmp2Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamAmp2Smooth].radix = SMOOTH_FREQ_RADIX;



}
