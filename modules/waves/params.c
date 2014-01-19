#include <string.h>

#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "module_custom.h"
#include "osc.h"
#include "params.h" 

void fill_param_desc(void) {

  strcpy(gModuleData->paramDesc[eParamHz0].label, "hz0");
  gModuleData->paramDesc[eParamHz0].type = eParamTypeNote;
  gModuleData->paramDesc[eParamHz0].min = OSC_HZ_MIN;
  gModuleData->paramDesc[eParamHz0].max = OSC_HZ_MAX;
  gModuleData->paramDesc[eParamHz0].radix = OSC_HZ_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamHz1].label, "hz1");
  gModuleData->paramDesc[eParamHz1].type = eParamTypeNote;
  gModuleData->paramDesc[eParamHz1].min = OSC_HZ_MIN;
  gModuleData->paramDesc[eParamHz1].max = OSC_HZ_MAX;
  gModuleData->paramDesc[eParamHz1].radix = OSC_HZ_RADIX;

  strcpy(gModuleData->paramDesc[eParamTune1].label, "tune1");
  gModuleData->paramDesc[eParamTune1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamTune1].min = RATIO_MIN;
  gModuleData->paramDesc[eParamTune1].max = RATIO_MAX;
  gModuleData->paramDesc[eParamTune1].radix = RATIO_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamTune0].label, "tune0");
  gModuleData->paramDesc[eParamTune0].type = eParamTypeFix;
  gModuleData->paramDesc[eParamTune0].min = RATIO_MIN;
  gModuleData->paramDesc[eParamTune0].max = RATIO_MAX;
  gModuleData->paramDesc[eParamTune0].radix = RATIO_RADIX;
    
  strcpy(gModuleData->paramDesc[eParamAmp1].label, "amp1");
  gModuleData->paramDesc[eParamAmp1].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamAmp1].min = 0;
  gModuleData->paramDesc[eParamAmp1].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamAmp1].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamAmp0].label, "amp0");
  gModuleData->paramDesc[eParamAmp0].type = eParamTypeAmp;
  gModuleData->paramDesc[eParamAmp0].min = 0;
  gModuleData->paramDesc[eParamAmp0].max = FRACT32_MAX >> 1;
  gModuleData->paramDesc[eParamAmp0].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamPm10].label, "pm10");
  gModuleData->paramDesc[eParamPm10].type = eParamTypeFix;
  gModuleData->paramDesc[eParamPm10].min = 0;
  gModuleData->paramDesc[eParamPm10].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamPm10].radix = 1;

  strcpy(gModuleData->paramDesc[eParamPm01].label, "pm01");
  gModuleData->paramDesc[eParamPm01].type = eParamTypeFix;
  gModuleData->paramDesc[eParamPm01].min = 0;
  gModuleData->paramDesc[eParamPm01].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamPm01].radix = 1;

  strcpy(gModuleData->paramDesc[eParamWm10].label, "wm10");
  gModuleData->paramDesc[eParamWm10].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWm10].min = 0;
  gModuleData->paramDesc[eParamWm10].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWm10].radix = 1;

  strcpy(gModuleData->paramDesc[eParamWm01].label, "wm01");
  gModuleData->paramDesc[eParamWm01].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWm01].min = 0;
  gModuleData->paramDesc[eParamWm01].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWm01].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamWave1].label, "wave1");
  gModuleData->paramDesc[eParamWave1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWave1].min = 0;
  gModuleData->paramDesc[eParamWave1].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWave1].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamWave0].label, "wave0");
  gModuleData->paramDesc[eParamWave0].type = eParamTypeFix;
  gModuleData->paramDesc[eParamWave0].min = 0;
  gModuleData->paramDesc[eParamWave0].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamWave0].radix = 1;
 
  strcpy(gModuleData->paramDesc[eParamBl1].label, "bl1");
  gModuleData->paramDesc[eParamBl1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamBl1].min = 0;
  gModuleData->paramDesc[eParamBl1].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamBl1].radix = 1;
  
  strcpy(gModuleData->paramDesc[eParamBl0].label, "bl0");
  gModuleData->paramDesc[eParamBl0].type = eParamTypeFix;
  gModuleData->paramDesc[eParamBl0].min = 0;
  gModuleData->paramDesc[eParamBl0].max = FRACT32_MAX;
  gModuleData->paramDesc[eParamBl0].radix = 1;


  strcpy(gModuleData->paramDesc[eParamFmDel1].label, "fmDel1");
  gModuleData->paramDesc[eParamFmDel1].type = eParamTypeFix;
  gModuleData->paramDesc[eParamFmDel1].min = PARAM_FM_DEL_MIN;
  gModuleData->paramDesc[eParamFmDel1].max = PARAM_FM_DEL_MAX;
  gModuleData->paramDesc[eParamFmDel1].radix = PARAM_FM_DEL_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamFmDel0].label, "fmDel0");
  gModuleData->paramDesc[eParamFmDel0].type = eParamTypeFix;
  gModuleData->paramDesc[eParamFmDel0].min = PARAM_FM_DEL_MIN;
  gModuleData->paramDesc[eParamFmDel0].max = PARAM_FM_DEL_MAX;
  gModuleData->paramDesc[eParamFmDel0].radix = PARAM_FM_DEL_RADIX;


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

  // svf 0
  strcpy(gModuleData->paramDesc[eParam_cut0].label, "cut0");
  gModuleData->paramDesc[eParam_cut0].type = eParamTypeSvfFreq; 
  gModuleData->paramDesc[eParam_cut0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cut0].max = PARAM_CUT_MAX;
  gModuleData->paramDesc[eParam_cut0].radix = 32;

  ///// FIXME
  strcpy(gModuleData->paramDesc[eParam_rq0].label, "rq0");
  gModuleData->paramDesc[eParam_rq0].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_rq0].min = PARAM_RQ_MIN;
  gModuleData->paramDesc[eParam_rq0].max = PARAM_RQ_MAX;
  gModuleData->paramDesc[eParam_rq0].radix = 2;
  
  strcpy(gModuleData->paramDesc[eParam_low0].label, "low0");
  gModuleData->paramDesc[eParam_low0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_low0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_low0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_low0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_high0].label, "high0");
  gModuleData->paramDesc[eParam_high0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_high0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_high0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_high0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_band0].label, "band0");
  gModuleData->paramDesc[eParam_band0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_band0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_band0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_band0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_notch0].label, "notch0");
  gModuleData->paramDesc[eParam_notch0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_notch0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_notch0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_notch0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_fwet0].label, "fwet0");
  gModuleData->paramDesc[eParam_fwet0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_fwet0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_fwet0].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_fwet0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_fdry0].label, "fdry0");
  gModuleData->paramDesc[eParam_fdry0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_fdry0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_fdry0].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_fdry0].radix = 1;

  ////////
  //////

  strcpy(gModuleData->paramDesc[eParamHz0Slew].label, "hz0Slew");
  gModuleData->paramDesc[eParamHz0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamHz0Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamHz0Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamHz0Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamHz1Slew].label, "hz1Slew");
  gModuleData->paramDesc[eParamHz1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamHz1Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamHz1Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamHz1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamPm10Slew].label, "pm10Slew");
  gModuleData->paramDesc[eParamPm10Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamPm10Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamPm10Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamPm10Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(gModuleData->paramDesc[eParamPm01Slew].label, "pm01Slew");
  gModuleData->paramDesc[eParamPm01Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamPm01Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamPm01Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamPm01Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(gModuleData->paramDesc[eParamWm10Slew].label, "wm10Slew");
  gModuleData->paramDesc[eParamWm10Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWm10Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamWm10Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamWm10Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(gModuleData->paramDesc[eParamWm01Slew].label, "wm01Slew");
  gModuleData->paramDesc[eParamWm01Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWm01Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamWm01Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamWm01Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(gModuleData->paramDesc[eParamWave1Slew].label, "wave1Slew");
  gModuleData->paramDesc[eParamWave1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWave1Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamWave1Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamWave1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamWave0Slew].label, "wave0Slew");
  gModuleData->paramDesc[eParamWave0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWave0Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamWave0Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamWave0Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamAmp1Slew].label, "amp1Slew");
  gModuleData->paramDesc[eParamAmp1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamAmp1Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamAmp1Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamAmp1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(gModuleData->paramDesc[eParamAmp0Slew].label, "amp0Slew");
  gModuleData->paramDesc[eParamAmp0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamAmp0Slew].min = SLEW_SECONDS_MIN;
  gModuleData->paramDesc[eParamAmp0Slew].max = SLEW_SECONDS_MAX;
  gModuleData->paramDesc[eParamAmp0Slew].radix = SLEW_SECONDS_RADIX;

  /* strcpy(gModuleData->paramDesc[eParamIoAmp0].label, "ioAmp1"); */
  /* gModuleData->paramDesc[eParamIoAmp0].type = eParamTypeAmp; */
  /* gModuleData->paramDesc[eParamIoAmp0].min = 0; */
  /* gModuleData->paramDesc[eParamIoAmp0].max = FRACT32_MAX >> 1; */
  /* gModuleData->paramDesc[eParamIoAmp0].radix = 32; */

  /* strcpy(gModuleData->paramDesc[eParamIoAmp1].label, "ioAmp2"); */
  /* gModuleData->paramDesc[eParamIoAmp1].type = eParamTypeAmp; */
  /* gModuleData->paramDesc[eParamIoAmp1].min = 0; */
  /* gModuleData->paramDesc[eParamIoAmp1].max = FRACT32_MAX >> 1; */
  /* gModuleData->paramDesc[eParamIoAmp1].radix = 32; */

  /* strcpy(gModuleData->paramDesc[eParamIoAmp2].label, "ioAmp3"); */
  /* gModuleData->paramDesc[eParamIoAmp2].type = eParamTypeAmp; */
  /* gModuleData->paramDesc[eParamIoAmp2].min = 0; */
  /* gModuleData->paramDesc[eParamIoAmp2].max = FRACT32_MAX >> 1; */
  /* gModuleData->paramDesc[eParamIoAmp2].radix = 32; */

  /* strcpy(gModuleData->paramDesc[eParamIoAmp3].label, "ioAmp4"); */
  /* gModuleData->paramDesc[eParamIoAmp3].type = eParamTypeAmp; */
  /* gModuleData->paramDesc[eParamIoAmp3].min = 0; */
  /* gModuleData->paramDesc[eParamIoAmp3].max = FRACT32_MAX >> 1; */
  /* gModuleData->paramDesc[eParamIoAmp3].radix = 32; */


  strcpy(gModuleData->paramDesc[eParam_cvVal0].label, "cv0");
  gModuleData->paramDesc[eParam_cvVal0].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_cvVal0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvVal0].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_cvVal0].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_cvVal1].label, "cv1");
  gModuleData->paramDesc[eParam_cvVal1].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_cvVal1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvVal1].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_cvVal1].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_cvVal2].label, "cv2");
  gModuleData->paramDesc[eParam_cvVal2].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_cvVal2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvVal2].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_cvVal2].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_cvVal3].label, "cv3");
  gModuleData->paramDesc[eParam_cvVal3].type = eParamTypeFix; 
  gModuleData->paramDesc[eParam_cvVal3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvVal3].max = PARAM_DAC_MAX;
  gModuleData->paramDesc[eParam_cvVal3].radix = PARAM_DAC_RADIX;

  strcpy(gModuleData->paramDesc[eParam_cvSlew0].label, "cvSlew0");
  gModuleData->paramDesc[eParam_cvSlew0].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_cvSlew0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvSlew0].max = 0x7fffffff;
  gModuleData->paramDesc[eParam_cvSlew0].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_cvSlew1].label, "cvSlew1");
  gModuleData->paramDesc[eParam_cvSlew1].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_cvSlew1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvSlew1].max = 0x7fffffff;
  gModuleData->paramDesc[eParam_cvSlew1].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_cvSlew2].label, "cvSlew2");
  gModuleData->paramDesc[eParam_cvSlew2].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_cvSlew2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvSlew2].max = 0x7fffffff; 
  gModuleData->paramDesc[eParam_cvSlew2].radix = 32;

  strcpy(gModuleData->paramDesc[eParam_cvSlew3].label, "cvSlew3");
  gModuleData->paramDesc[eParam_cvSlew3].type = eParamTypeIntegrator; 
  gModuleData->paramDesc[eParam_cvSlew3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_cvSlew3].max = 0x7fffffff;
  gModuleData->paramDesc[eParam_cvSlew3].radix = 32;

  // osc mix

  strcpy(gModuleData->paramDesc[eParam_osc0_dac0].label, "osc0_dac0");
  gModuleData->paramDesc[eParam_osc0_dac0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc0_dac0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc0_dac0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc0_dac0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc0_dac1].label, "osc0_dac1");
  gModuleData->paramDesc[eParam_osc0_dac1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc0_dac1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc0_dac1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc0_dac1].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc0_dac2].label, "osc0_dac2");
  gModuleData->paramDesc[eParam_osc0_dac2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc0_dac2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc0_dac2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc0_dac2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc0_dac3].label, "osc0_dac3");
  gModuleData->paramDesc[eParam_osc0_dac3].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc0_dac3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc0_dac3].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc0_dac3].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc1_dac0].label, "osc1_dac0");
  gModuleData->paramDesc[eParam_osc1_dac0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc1_dac0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc1_dac0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc1_dac0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc1_dac1].label, "osc1_dac1");
  gModuleData->paramDesc[eParam_osc1_dac1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc1_dac1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc1_dac1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc1_dac1].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc1_dac2].label, "osc1_dac2");
  gModuleData->paramDesc[eParam_osc1_dac2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc1_dac2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc1_dac2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_osc1_dac2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_osc1_dac3].label, "osc1_dac3");
  gModuleData->paramDesc[eParam_osc1_dac3].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_osc1_dac3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_osc1_dac3].max = FR32_MAX; 
  gModuleData->paramDesc[eParam_osc1_dac3].radix = 1;

  // i/o mix
  strcpy(gModuleData->paramDesc[eParam_adc0_dac0].label, "adc0_dac0");
  gModuleData->paramDesc[eParam_adc0_dac0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc0_dac0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc0_dac0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc0_dac0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc0_dac1].label, "adc0_dac1");
  gModuleData->paramDesc[eParam_adc0_dac1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc0_dac1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc0_dac1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc0_dac1].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc0_dac2].label, "adc0_dac2");
  gModuleData->paramDesc[eParam_adc0_dac2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc0_dac2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc0_dac2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc0_dac2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc0_dac3].label, "adc0_dac3");
  gModuleData->paramDesc[eParam_adc0_dac3].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc0_dac3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc0_dac3].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc0_dac3].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc1_dac0].label, "adc1_dac0");
  gModuleData->paramDesc[eParam_adc1_dac0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc1_dac0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc1_dac0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc1_dac0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc1_dac1].label, "adc1_dac1");
  gModuleData->paramDesc[eParam_adc1_dac1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc1_dac1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc1_dac1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc1_dac1].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc1_dac2].label, "adc1_dac2");
  gModuleData->paramDesc[eParam_adc1_dac2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc1_dac2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc1_dac2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc1_dac2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc1_dac3].label, "adc1_dac3");
  gModuleData->paramDesc[eParam_adc1_dac3].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc1_dac3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc1_dac3].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc1_dac3].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc2_dac0].label, "adc2_dac0");
  gModuleData->paramDesc[eParam_adc2_dac0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc2_dac0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc2_dac0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc2_dac0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc2_dac1].label, "adc2_dac1");
  gModuleData->paramDesc[eParam_adc2_dac1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc2_dac1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc2_dac1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc2_dac1].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc2_dac2].label, "adc2_dac2");
  gModuleData->paramDesc[eParam_adc2_dac2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc2_dac2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc2_dac2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc2_dac2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc2_dac3].label, "adc2_dac3");
  gModuleData->paramDesc[eParam_adc2_dac3].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc2_dac3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc2_dac3].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc2_dac3].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc3_dac0].label, "adc3_dac0");
  gModuleData->paramDesc[eParam_adc3_dac0].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc3_dac0].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc3_dac0].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc3_dac0].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc3_dac1].label, "adc3_dac1");
  gModuleData->paramDesc[eParam_adc3_dac1].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc3_dac1].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc3_dac1].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc3_dac1].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc3_dac2].label, "adc3_dac2");
  gModuleData->paramDesc[eParam_adc3_dac2].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc3_dac2].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc3_dac2].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc3_dac2].radix = 1;

  strcpy(gModuleData->paramDesc[eParam_adc3_dac3].label, "adc3_dac3");
  gModuleData->paramDesc[eParam_adc3_dac3].type = eParamTypeAmp; 
  gModuleData->paramDesc[eParam_adc3_dac3].min = 0x00000000; 
  gModuleData->paramDesc[eParam_adc3_dac3].max = FR32_MAX;
  gModuleData->paramDesc[eParam_adc3_dac3].radix = 1;


  // filter slew

  strcpy(gModuleData->paramDesc[eParamCut0Slew].label, "cut0Slew");
  gModuleData->paramDesc[eParamCut0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamCut0Slew].min = 0;
  gModuleData->paramDesc[eParamCut0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamCut0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamRq0Slew].label, "rq0Slew");
  gModuleData->paramDesc[eParamRq0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamRq0Slew].min = 0;
  gModuleData->paramDesc[eParamRq0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamRq0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamLow0Slew].label, "low0Slew");
  gModuleData->paramDesc[eParamLow0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamLow0Slew].min = 0;
  gModuleData->paramDesc[eParamLow0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamLow0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamHigh0Slew].label, "high0Slew");
  gModuleData->paramDesc[eParamHigh0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamHigh0Slew].min = 0;
  gModuleData->paramDesc[eParamHigh0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamHigh0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamBand0Slew].label, "band0Slew");
  gModuleData->paramDesc[eParamBand0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamBand0Slew].min = 0;
  gModuleData->paramDesc[eParamBand0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamBand0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamNotch0Slew].label, "notch0Slew");
  gModuleData->paramDesc[eParamNotch0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamNotch0Slew].min = 0;
  gModuleData->paramDesc[eParamNotch0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamNotch0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamDry0Slew].label, "dry0Slew");
  gModuleData->paramDesc[eParamDry0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamDry0Slew].min = 0;
  gModuleData->paramDesc[eParamDry0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamDry0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamWet0Slew].label, "wet0Slew");
  gModuleData->paramDesc[eParamWet0Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWet0Slew].min = 0;
  gModuleData->paramDesc[eParamWet0Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamWet0Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamCut1Slew].label, "cut1Slew");
  gModuleData->paramDesc[eParamCut1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamCut1Slew].min = 0;
  gModuleData->paramDesc[eParamCut1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamCut1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamRq1Slew].label, "rq1Slew");
  gModuleData->paramDesc[eParamRq1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamRq1Slew].min = 0;
  gModuleData->paramDesc[eParamRq1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamRq1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamLow1Slew].label, "low1Slew");
  gModuleData->paramDesc[eParamLow1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamLow1Slew].min = 0;
  gModuleData->paramDesc[eParamLow1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamLow1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamHigh1Slew].label, "high1Slew");
  gModuleData->paramDesc[eParamHigh1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamHigh1Slew].min = 0;
  gModuleData->paramDesc[eParamHigh1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamHigh1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamBand1Slew].label, "band1Slew");
  gModuleData->paramDesc[eParamBand1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamBand1Slew].min = 0;
  gModuleData->paramDesc[eParamBand1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamBand1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamNotch1Slew].label, "notch1Slew");
  gModuleData->paramDesc[eParamNotch1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamNotch1Slew].min = 0;
  gModuleData->paramDesc[eParamNotch1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamNotch1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamDry1Slew].label, "dry1Slew");
  gModuleData->paramDesc[eParamDry1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamDry1Slew].min = 0;
  gModuleData->paramDesc[eParamDry1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamDry1Slew].radix = 32;

  strcpy(gModuleData->paramDesc[eParamWet1Slew].label, "wet1Slew");
  gModuleData->paramDesc[eParamWet1Slew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamWet1Slew].min = 0;
  gModuleData->paramDesc[eParamWet1Slew].max = FR32_MAX;
  gModuleData->paramDesc[eParamWet1Slew].radix = 32;


  strcpy(gModuleData->paramDesc[eParamMixSlew].label, "mixSlew");
  gModuleData->paramDesc[eParamMixSlew].type = eParamTypeIntegrator;
  gModuleData->paramDesc[eParamMixSlew].min = 0;
  gModuleData->paramDesc[eParamMixSlew].max = FR32_MAX;
  gModuleData->paramDesc[eParamMixSlew].radix = 32;


}
