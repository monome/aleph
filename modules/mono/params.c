#include <string.h>

#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "params.h" 

void fill_param_desc(void) {
  strcpy(gModuleData->paramDesc[eParamFreq1].label, "osc1_hz");
  gModuleData->paramDesc[eParamFreq1].type = eParamTypeNote;
  gModuleData->paramDesc[eParamFreq1].min = OSC_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq1].max = OSC_FREQ_MAX;
  gModuleData->paramDesc[eParamFreq1].radix = OSC_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamFreq2].label, "osc2_hz");
  gModuleData->paramDesc[eParamFreq2].type = eParamTypeNote;
  gModuleData->paramDesc[eParamFreq2].min = OSC_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq2].max = OSC_FREQ_MAX;
  gModuleData->paramDesc[eParamFreq2].radix = OSC_FREQ_RADIX;
  
  /* strcpy(gModuleData->paramDesc[eParamRatio2].label, "osc2_ratio"); */
  /* gModuleData->paramDesc[eParamRatio2].type = eParamTypeFix; */
  /* gModuleData->paramDesc[eParamRatio2].min = RATIO_MIN; */
  /* gModuleData->paramDesc[eParamRatio2].max = RATIO_MAX; */
  /* gModuleData->paramDesc[eParamRatio2].radix = RATIO_RADIX; */
  
  /* strcpy(gModuleData->paramDesc[eParamTrack].label, "osc2_track"); */
  /* gModuleData->paramDesc[eParamTrack].type = eParamTypeBool; */
  /* gModuleData->paramDesc[eParamTrack].min = 0; */
  /* gModuleData->paramDesc[eParamTrack].max = 1; */
  /* gModuleData->paramDesc[eParamTrack].radix = 2; */
  
  strcpy(gModuleData->paramDesc[eParamAmp1].label, "amplitude 1");
  gModuleData->paramDesc[eParamAmp1].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamAmp1].min = 0;
  gModuleData->paramDesc[eParamAmp1].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamAmp1].radix = 32;
  
  strcpy(gModuleData->paramDesc[eParamAmp2].label, "amplitude 2");
  gModuleData->paramDesc[eParamAmp2].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamAmp2].min = 0;
  gModuleData->paramDesc[eParamAmp2].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamAmp2].radix = 32;
  
  strcpy(gModuleData->paramDesc[eParamIoAmp0].label, "I/O amp 1");
  gModuleData->paramDesc[eParamIoAmp0].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp0].min = 0;
  gModuleData->paramDesc[eParamIoAmp0].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp0].radix = 32;

  strcpy(gModuleData->paramDesc[eParamIoAmp1].label, "I/O amp 2");
  gModuleData->paramDesc[eParamIoAmp1].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp1].min = 0;
  gModuleData->paramDesc[eParamIoAmp1].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp1].radix = 32;

  strcpy(gModuleData->paramDesc[eParamIoAmp2].label, "I/O amp 3");
  gModuleData->paramDesc[eParamIoAmp2].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp2].min = 0;
  gModuleData->paramDesc[eParamIoAmp2].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp2].radix = 32;

  strcpy(gModuleData->paramDesc[eParamIoAmp3].label, "I/O amp 4");
  gModuleData->paramDesc[eParamIoAmp3].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamIoAmp3].min = 0;
  gModuleData->paramDesc[eParamIoAmp3].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamIoAmp3].radix = 32;

  strcpy(gModuleData->paramDesc[eParamPm].label, "phase mod depth");
  gModuleData->paramDesc[eParamPm].type = eParamTypeFix;
  gModuleData->paramDesc[eParamPm].min = 0;
  gModuleData->paramDesc[eParamPm].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamPm].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamWave1].label, "waveshape 1");
  gModuleData->paramDesc[eParamWave1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWave1].min = 0;
  gModuleData->paramDesc[eParamWave1].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWave1].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamWave2].label, "waveshape 2");
  gModuleData->paramDesc[eParamWave2].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWave2].min = 0;
  gModuleData->paramDesc[eParamWave2].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWave2].radix = 1;
  
  /* strcpy(gModuleData->paramDesc[eParamGate].label, "gate"); */
  /* gModuleData->paramDesc[eParamGate].type = eParamTypeFix; */
  /* gModuleData->paramDesc[eParamGate].min = 0; */
  /* gModuleData->paramDesc[eParamGate].max = 1;    //??? */
  /* gModuleData->paramDesc[eParamGate].radix = 32; */
  
  /* strcpy(gModuleData->paramDesc[eParamAtkDur].label, "amp env atk"); */
  /* gModuleData->paramDesc[eParamAtkDur].type = eParamTypeFix; */
  /* gModuleData->paramDesc[eParamAtkDur].min = 0; */
  /* gModuleData->paramDesc[eParamAtkDur].max = ENV_DUR_MAX; */
  /* gModuleData->paramDesc[eParamAtkDur].radix = ENV_DUR_RADIX; */
  
  /* strcpy(gModuleData->paramDesc[eParamRelDur].label, "amp env rel"); */
  /* gModuleData->paramDesc[eParamRelDur].type = eParamTypeFix; */
  /* gModuleData->paramDesc[eParamRelDur].min = 0; */
  /* gModuleData->paramDesc[eParamRelDur].max = ENV_DUR_MAX; */
  /* gModuleData->paramDesc[eParamRelDur].radix = 6; */
  
  /* strcpy(gModuleData->paramDesc[eParamAtkCurve].label, "amp env atk curve"); */
  /* gModuleData->paramDesc[eParamAtkCurve].type = eParamTypeFix; */
  /* gModuleData->paramDesc[eParamAtkCurve].min = 0; */
  /* gModuleData->paramDesc[eParamAtkCurve].max = FRACT32_MAX; */
  /* gModuleData->paramDesc[eParamAtkCurve].radix = 1; */
  
  /* strcpy(gModuleData->paramDesc[eParamRelCurve].label, "amp env rel curve"); */
  /* gModuleData->paramDesc[eParamRelCurve].type = eParamTypeFix; */
  /* gModuleData->paramDesc[eParamRelCurve].min = 0; */
  /* gModuleData->paramDesc[eParamRelCurve].max = FRACT32_MAX; */
  /* gModuleData->paramDesc[eParamRelCurve].radix = 1; */
  
  strcpy(gModuleData->paramDesc[eParamFreq1Smooth].label, "freq 1 smooth");
  gModuleData->paramDesc[eParamFreq1Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamFreq1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq1Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamFreq1Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamFreq2Smooth].label, "freq 2 smooth");
  gModuleData->paramDesc[eParamFreq2Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamFreq2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq2Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamFreq2Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamPmSmooth].label, "phase mod smooth");
  gModuleData->paramDesc[eParamPmSmooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamPmSmooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamPmSmooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamPmSmooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamWave1Smooth].label, "wave 1 smooth");
  gModuleData->paramDesc[eParamWave1Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWave1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWave1Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamWave1Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamWave2Smooth].label, "wave 2 smooth");
  gModuleData->paramDesc[eParamWave2Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWave2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWave2Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamWave2Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamAmp1Smooth].label, "amp 1 smooth");
  gModuleData->paramDesc[eParamAmp1Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamAmp1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamAmp1Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamAmp1Smooth].radix = SMOOTH_FREQ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamAmp2Smooth].label, "amp 2 smooth");
  gModuleData->paramDesc[eParamAmp2Smooth].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamAmp2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamAmp2Smooth].max = SMOOTH_FREQ_MAX;
  gModuleData->paramDesc[eParamAmp2Smooth].radix = SMOOTH_FREQ_RADIX;
}
