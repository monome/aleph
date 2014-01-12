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

  /////////////////////
  /////////////

  // svf 1
  strcpy(gModuleData->paramDesc[eParam_cut1].label, "cut1");
  gModuleData->paramDesc[eParam_cut1].type = eParamTypeSvfFreq; 
  gModuleData->paramDesc[eParam_cut1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cut1].max = PARAM_CUT_MAX;
  gModuleData->paramDesc[eParam_cut1].radix = 32;

  ///// FIXME
  strcpy(gModuleData->paramDesc[eParam_rq1].label, "rq1");
  gModuleData->paramDesc[eParam_rq1].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_rq1].min = PARAM_RQ_MIN;
  gModuleData->paramDesc[eParam_rq1].max = PARAM_RQ_MAX;
  gModuleData->paramDesc[eParam_rq1].radix = 2;
  
  strcpy(gModuleData->paramDesc[eParam_low1].label, "low1");
  gModuleData->paramDesc[eParam_low1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_low1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_low1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_low1].radix = 0;

  strcpy(gModuleData->paramDesc[eParam_high1].label, "high1");
  gModuleData->paramDesc[eParam_high1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_high1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_high1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_high1].radix = 0;

  strcpy(gModuleData->paramDesc[eParam_band1].label, "band1");
  gModuleData->paramDesc[eParam_band1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_band1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_band1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_band1].radix = 0;

  strcpy(gModuleData->paramDesc[eParam_notch1].label, "notch1");
  gModuleData->paramDesc[eParam_notch1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_notch1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_notch1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_notch1].radix = 0;

  strcpy(gModuleData->paramDesc[eParam_fwet1].label, "fwet1");
  gModuleData->paramDesc[eParam_fwet1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_fwet1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_fwet1].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_fwet1].radix = 0;

  strcpy(gModuleData->paramDesc[eParam_fdry1].label, "fdry1");
  gModuleData->paramDesc[eParam_fdry1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_fdry1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_fdry1].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_fdry1].radix = 0;

  // svf 2
  strcpy(gModuleData->paramDesc[eParam_cut2].label, "cut2");
  gModuleData->paramDesc[eParam_cut2].type = eParamTypeSvfFreq; 
  gModuleData->paramDesc[eParam_cut2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cut2].max = PARAM_CUT_MAX;
  gModuleData->paramDesc[eParam_cut2].radix = 32;

  ///// FIXME
  strcpy(gModuleData->paramDesc[eParam_rq2].label, "rq2");
  gModuleData->paramDesc[eParam_rq2].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_rq2].min = PARAM_RQ_MIN;
  gModuleData->paramDesc[eParam_rq2].max = PARAM_RQ_MAX;
  gModuleData->paramDesc[eParam_rq2].radix = 2;
  
  strcpy(gModuleData->paramDesc[eParam_low2].label, "low2");
  gModuleData->paramDesc[eParam_low2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_low2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_low2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_low2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_high2].label, "high2");
  gModuleData->paramDesc[eParam_high2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_high2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_high2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_high2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_band2].label, "band2");
  gModuleData->paramDesc[eParam_band2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_band2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_band2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_band2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_notch2].label, "notch2");
  gModuleData->paramDesc[eParam_notch2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_notch2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_notch2].max = 0x00010000;
  gModuleData->paramDesc[eParam_notch2].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_fwet2].label, "fwet2");
  gModuleData->paramDesc[eParam_fwet2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_fwet2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_fwet2].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_fwet2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_fdry2].label, "fdry2");
  gModuleData->paramDesc[eParam_fdry2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_fdry2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_fdry2].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_fdry2].radix = 1;

  ////////
  //////

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


  strcpy(gModuleData->paramDesc[eParam_dacVal0].label, "dac0");
  gModuleData->paramDesc[eParam_dacVal0].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_dacVal0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacVal0].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_dacVal0].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_dacVal1].label, "dac1");
  gModuleData->paramDesc[eParam_dacVal1].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_dacVal1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacVal1].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_dacVal1].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_dacVal2].label, "dac2");
  gModuleData->paramDesc[eParam_dacVal2].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_dacVal2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacVal2].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_dacVal2].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_dacVal3].label, "dac3");
  gModuleData->paramDesc[eParam_dacVal3].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_dacVal3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacVal3].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_dacVal3].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_dacSlew0].label, "dacSlew0");
  gModuleData->paramDesc[eParam_dacSlew0].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_dacSlew0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacSlew0].max = 0x7fffffff;
  gModuleData->paramDesc[eParam_dacSlew0].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_dacSlew1].label, "dacSlew1");
  gModuleData->paramDesc[eParam_dacSlew1].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_dacSlew1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacSlew1].max = 0x7fffffff;
  gModuleData->paramDesc[eParam_dacSlew1].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_dacSlew2].label, "dacSlew2");
  gModuleData->paramDesc[eParam_dacSlew2].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_dacSlew2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacSlew2].max = 0x7fffffff; 
  gModuleData->paramDesc[eParam_dacSlew2].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_dacSlew3].label, "dacSlew3");
  gModuleData->paramDesc[eParam_dacSlew3].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_dacSlew3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_dacSlew3].max = 0x7fffffff;
  gModuleData->paramDesc[eParam_dacSlew3].radix = 32;


}
