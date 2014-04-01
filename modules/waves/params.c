#include <string.h>
#include "param_common.h"
#include "params.h"

void fill_param_desc(ParamDesc* desc) {

  strcpy(desc[eParamHz0].label, "hz0");
  desc[eParamHz0].type = eParamTypeNote;
  desc[eParamHz0].min = OSC_HZ_MIN;
  desc[eParamHz0].max = OSC_HZ_MAX;
  desc[eParamHz0].radix = OSC_HZ_RADIX;
  
  strcpy(desc[eParamHz1].label, "hz1");
  desc[eParamHz1].type = eParamTypeNote;
  desc[eParamHz1].min = OSC_HZ_MIN;
  desc[eParamHz1].max = OSC_HZ_MAX;
  desc[eParamHz1].radix = OSC_HZ_RADIX;

  strcpy(desc[eParamTune1].label, "tune1");
  desc[eParamTune1].type = eParamTypeFix;
  desc[eParamTune1].min = RATIO_MIN;
  desc[eParamTune1].max = RATIO_MAX;
  desc[eParamTune1].radix = RATIO_RADIX;
  
  strcpy(desc[eParamTune0].label, "tune0");
  desc[eParamTune0].type = eParamTypeFix;
  desc[eParamTune0].min = RATIO_MIN;
  desc[eParamTune0].max = RATIO_MAX;
  desc[eParamTune0].radix = RATIO_RADIX;
    
  strcpy(desc[eParamAmp1].label, "amp1");
  desc[eParamAmp1].type = eParamTypeAmp;
  desc[eParamAmp1].min = 0;
  desc[eParamAmp1].max = FRACT32_MAX >> 1;
  desc[eParamAmp1].radix = 1;
  
  strcpy(desc[eParamAmp0].label, "amp0");
  desc[eParamAmp0].type = eParamTypeAmp;
  desc[eParamAmp0].min = 0;
  desc[eParamAmp0].max = FRACT32_MAX >> 1;
  desc[eParamAmp0].radix = 1;
  
  strcpy(desc[eParamPm10].label, "pm10");
  desc[eParamPm10].type = eParamTypeFix;
  desc[eParamPm10].min = 0;
  desc[eParamPm10].max = FRACT32_MAX;
  desc[eParamPm10].radix = 1;

  strcpy(desc[eParamPm01].label, "pm01");
  desc[eParamPm01].type = eParamTypeFix;
  desc[eParamPm01].min = 0;
  desc[eParamPm01].max = FRACT32_MAX;
  desc[eParamPm01].radix = 1;

  strcpy(desc[eParamWm10].label, "wm10");
  desc[eParamWm10].type = eParamTypeFix;
  desc[eParamWm10].min = 0;
  desc[eParamWm10].max = FRACT32_MAX;
  desc[eParamWm10].radix = 1;

  strcpy(desc[eParamWm01].label, "wm01");
  desc[eParamWm01].type = eParamTypeFix;
  desc[eParamWm01].min = 0;
  desc[eParamWm01].max = FRACT32_MAX;
  desc[eParamWm01].radix = 1;
  
  strcpy(desc[eParamWave1].label, "wave1");
  desc[eParamWave1].type = eParamTypeFix;
  desc[eParamWave1].min = 0;
  desc[eParamWave1].max = FRACT32_MAX;
  desc[eParamWave1].radix = 1;
  
  strcpy(desc[eParamWave0].label, "wave0");
  desc[eParamWave0].type = eParamTypeFix;
  desc[eParamWave0].min = 0;
  desc[eParamWave0].max = FRACT32_MAX;
  desc[eParamWave0].radix = 1;

  strcpy(desc[eParamFmDel1].label, "modDel1");
  desc[eParamFmDel1].type = eParamTypeFix;
  desc[eParamFmDel1].min = PARAM_FM_DEL_MIN;
  desc[eParamFmDel1].max = PARAM_FM_DEL_MAX;
  desc[eParamFmDel1].radix = PARAM_FM_DEL_RADIX;
  
  strcpy(desc[eParamFmDel0].label, "modDel0");
  desc[eParamFmDel0].type = eParamTypeFix;
  desc[eParamFmDel0].min = PARAM_FM_DEL_MIN;
  desc[eParamFmDel0].max = PARAM_FM_DEL_MAX;
  desc[eParamFmDel0].radix = PARAM_FM_DEL_RADIX;


  /////////////////////
  /////////////

  // svf 1
  strcpy(desc[eParam_cut1].label, "cut1");
  desc[eParam_cut1].type = eParamTypeSvfFreq; 
  desc[eParam_cut1].min = 0x00000000; 
  desc[eParam_cut1].max = PARAM_CUT_MAX;
  desc[eParam_cut1].radix = 32;

  ///// FIXME
  strcpy(desc[eParam_rq1].label, "rq1");
  desc[eParam_rq1].type = eParamTypeFix; 
  desc[eParam_rq1].min = PARAM_RQ_MIN;
  desc[eParam_rq1].max = PARAM_RQ_MAX;
  desc[eParam_rq1].radix = 2;
  
  strcpy(desc[eParam_fwet1].label, "fwet1");
  desc[eParam_fwet1].type = eParamTypeAmp; 
  desc[eParam_fwet1].min = 0x00000000; 
  desc[eParam_fwet1].max = FR32_MAX; 
  desc[eParam_fwet1].radix = 0;

  strcpy(desc[eParam_fdry1].label, "fdry1");
  desc[eParam_fdry1].type = eParamTypeAmp; 
  desc[eParam_fdry1].min = 0x00000000; 
  desc[eParam_fdry1].max = FR32_MAX; 
  desc[eParam_fdry1].radix = 0;

  // svf 0
  strcpy(desc[eParam_cut0].label, "cut0");
  desc[eParam_cut0].type = eParamTypeSvfFreq; 
  desc[eParam_cut0].min = 0x00000000; 
  desc[eParam_cut0].max = PARAM_CUT_MAX;
  desc[eParam_cut0].radix = 32;

  ///// FIXME
  strcpy(desc[eParam_rq0].label, "rq0");
  desc[eParam_rq0].type = eParamTypeFix; 
  desc[eParam_rq0].min = PARAM_RQ_MIN;
  desc[eParam_rq0].max = PARAM_RQ_MAX;
  desc[eParam_rq0].radix = 2;

  strcpy(desc[eParam_mode0].label, "mode0");
  desc[eParam_mode0].type = eParamTypeFix; 
  desc[eParam_mode0].min = PARAM_MODE_MIN; 
  desc[eParam_mode0].max = PARAM_MODE_MAX; 
  desc[eParam_mode0].radix = PARAM_MODE_RADIX;

  strcpy(desc[eParam_mode1].label, "mode1");
  desc[eParam_mode1].type = eParamTypeFix; 
  desc[eParam_mode1].min = PARAM_MODE_MIN; 
  desc[eParam_mode1].max = PARAM_MODE_MAX; 
  desc[eParam_mode1].radix = PARAM_MODE_RADIX;

  strcpy(desc[eParam_fwet0].label, "fwet0");
  desc[eParam_fwet0].type = eParamTypeAmp; 
  desc[eParam_fwet0].min = 0x00000000; 
  desc[eParam_fwet0].max = FR32_MAX; 
  desc[eParam_fwet0].radix = 1;

  strcpy(desc[eParam_fdry0].label, "fdry0");
  desc[eParam_fdry0].type = eParamTypeAmp; 
  desc[eParam_fdry0].min = 0x00000000; 
  desc[eParam_fdry0].max = FR32_MAX; 
  desc[eParam_fdry0].radix = 1;

  strcpy(desc[eParamHz0Slew].label, "hz0Slew");
  desc[eParamHz0Slew].type = eParamTypeIntegrator;
  desc[eParamHz0Slew].min = SLEW_SECONDS_MIN;
  desc[eParamHz0Slew].max = SLEW_SECONDS_MAX;
  desc[eParamHz0Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(desc[eParamHz1Slew].label, "hz1Slew");
  desc[eParamHz1Slew].type = eParamTypeIntegrator;
  desc[eParamHz1Slew].min = SLEW_SECONDS_MIN;
  desc[eParamHz1Slew].max = SLEW_SECONDS_MAX;
  desc[eParamHz1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(desc[eParamPm10Slew].label, "pm10Slew");
  desc[eParamPm10Slew].type = eParamTypeIntegrator;
  desc[eParamPm10Slew].min = SLEW_SECONDS_MIN;
  desc[eParamPm10Slew].max = SLEW_SECONDS_MAX;
  desc[eParamPm10Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(desc[eParamPm01Slew].label, "pm01Slew");
  desc[eParamPm01Slew].type = eParamTypeIntegrator;
  desc[eParamPm01Slew].min = SLEW_SECONDS_MIN;
  desc[eParamPm01Slew].max = SLEW_SECONDS_MAX;
  desc[eParamPm01Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(desc[eParamWm10Slew].label, "wm10Slew");
  desc[eParamWm10Slew].type = eParamTypeIntegrator;
  desc[eParamWm10Slew].min = SLEW_SECONDS_MIN;
  desc[eParamWm10Slew].max = SLEW_SECONDS_MAX;
  desc[eParamWm10Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(desc[eParamWm01Slew].label, "wm01Slew");
  desc[eParamWm01Slew].type = eParamTypeIntegrator;
  desc[eParamWm01Slew].min = SLEW_SECONDS_MIN;
  desc[eParamWm01Slew].max = SLEW_SECONDS_MAX;
  desc[eParamWm01Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(desc[eParamWave1Slew].label, "wave1Slew");
  desc[eParamWave1Slew].type = eParamTypeIntegrator;
  desc[eParamWave1Slew].min = SLEW_SECONDS_MIN;
  desc[eParamWave1Slew].max = SLEW_SECONDS_MAX;
  desc[eParamWave1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(desc[eParamWave0Slew].label, "wave0Slew");
  desc[eParamWave0Slew].type = eParamTypeIntegrator;
  desc[eParamWave0Slew].min = SLEW_SECONDS_MIN;
  desc[eParamWave0Slew].max = SLEW_SECONDS_MAX;
  desc[eParamWave0Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(desc[eParamAmp1Slew].label, "amp1Slew");
  desc[eParamAmp1Slew].type = eParamTypeIntegrator;
  desc[eParamAmp1Slew].min = SLEW_SECONDS_MIN;
  desc[eParamAmp1Slew].max = SLEW_SECONDS_MAX;
  desc[eParamAmp1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(desc[eParamAmp0Slew].label, "amp0Slew");
  desc[eParamAmp0Slew].type = eParamTypeIntegrator;
  desc[eParamAmp0Slew].min = SLEW_SECONDS_MIN;
  desc[eParamAmp0Slew].max = SLEW_SECONDS_MAX;
  desc[eParamAmp0Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(desc[eParam_cvVal0].label, "cv0");
  desc[eParam_cvVal0].type = eParamTypeFix; 
  desc[eParam_cvVal0].min = 0x00000000; 
  desc[eParam_cvVal0].max = PARAM_DAC_MAX;
  desc[eParam_cvVal0].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvVal1].label, "cv1");
  desc[eParam_cvVal1].type = eParamTypeFix; 
  desc[eParam_cvVal1].min = 0x00000000; 
  desc[eParam_cvVal1].max = PARAM_DAC_MAX;
  desc[eParam_cvVal1].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvVal2].label, "cv2");
  desc[eParam_cvVal2].type = eParamTypeFix; 
  desc[eParam_cvVal2].min = 0x00000000; 
  desc[eParam_cvVal2].max = PARAM_DAC_MAX;
  desc[eParam_cvVal2].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvVal3].label, "cv3");
  desc[eParam_cvVal3].type = eParamTypeFix; 
  desc[eParam_cvVal3].min = 0x00000000; 
  desc[eParam_cvVal3].max = PARAM_DAC_MAX;
  desc[eParam_cvVal3].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvSlew0].label, "cvSlew0");
  desc[eParam_cvSlew0].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew0].min = 0x00000000; 
  desc[eParam_cvSlew0].max = 0x7fffffff;
  desc[eParam_cvSlew0].radix = 32;

  strcpy(desc[eParam_cvSlew1].label, "cvSlew1");
  desc[eParam_cvSlew1].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew1].min = 0x00000000; 
  desc[eParam_cvSlew1].max = 0x7fffffff;
  desc[eParam_cvSlew1].radix = 32;

  strcpy(desc[eParam_cvSlew2].label, "cvSlew2");
  desc[eParam_cvSlew2].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew2].min = 0x00000000; 
  desc[eParam_cvSlew2].max = 0x7fffffff; 
  desc[eParam_cvSlew2].radix = 32;

  strcpy(desc[eParam_cvSlew3].label, "cvSlew3");
  desc[eParam_cvSlew3].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew3].min = 0x00000000; 
  desc[eParam_cvSlew3].max = 0x7fffffff;
  desc[eParam_cvSlew3].radix = 32;

  // osc mix

  strcpy(desc[eParam_osc0_dac0].label, "osc0_dac0");
  desc[eParam_osc0_dac0].type = eParamTypeBool; 
  desc[eParam_osc0_dac0].min = 0x00000000; 
  desc[eParam_osc0_dac0].max = 1;
  desc[eParam_osc0_dac0].radix = 2;

  strcpy(desc[eParam_osc0_dac1].label, "osc0_dac1");
  desc[eParam_osc0_dac1].type = eParamTypeBool; 
  desc[eParam_osc0_dac1].min = 0x00000000; 
  desc[eParam_osc0_dac1].max = 1;
  desc[eParam_osc0_dac1].radix = 2;

  strcpy(desc[eParam_osc0_dac2].label, "osc0_dac2");
  desc[eParam_osc0_dac2].type = eParamTypeBool; 
  desc[eParam_osc0_dac2].min = 0x00000000; 
  desc[eParam_osc0_dac2].max = 1;
  desc[eParam_osc0_dac2].radix = 2;

  strcpy(desc[eParam_osc0_dac3].label, "osc0_dac3");
  desc[eParam_osc0_dac3].type = eParamTypeBool; 
  desc[eParam_osc0_dac3].min = 0x00000000; 
  desc[eParam_osc0_dac3].max = 1;
  desc[eParam_osc0_dac3].radix = 2;

  strcpy(desc[eParam_osc1_dac0].label, "osc1_dac0");
  desc[eParam_osc1_dac0].type = eParamTypeBool; 
  desc[eParam_osc1_dac0].min = 0x00000000; 
  desc[eParam_osc1_dac0].max = 1;
  desc[eParam_osc1_dac0].radix = 2;

  strcpy(desc[eParam_osc1_dac1].label, "osc1_dac1");
  desc[eParam_osc1_dac1].type = eParamTypeBool; 
  desc[eParam_osc1_dac1].min = 0x00000000; 
  desc[eParam_osc1_dac1].max = 1;
  desc[eParam_osc1_dac1].radix = 2;

  strcpy(desc[eParam_osc1_dac2].label, "osc1_dac2");
  desc[eParam_osc1_dac2].type = eParamTypeBool; 
  desc[eParam_osc1_dac2].min = 0x00000000; 
  desc[eParam_osc1_dac2].max = 1;
  desc[eParam_osc1_dac2].radix = 2;

  strcpy(desc[eParam_osc1_dac3].label, "osc1_dac3");
  desc[eParam_osc1_dac3].type = eParamTypeBool; 
  desc[eParam_osc1_dac3].min = 0x00000000; 
  desc[eParam_osc1_dac3].max = 1; 
  desc[eParam_osc1_dac3].radix = 2;


  // i/o patch points
  strcpy(desc[eParam_adc0_dac0].label, "adc0_dac0");
  desc[eParam_adc0_dac0].type = eParamTypeBool; 
  desc[eParam_adc0_dac0].min = 0x00000000; 
  desc[eParam_adc0_dac0].max = 1;
  desc[eParam_adc0_dac0].radix = 2;

  strcpy(desc[eParam_adc0_dac1].label, "adc0_dac1");
  desc[eParam_adc0_dac1].type = eParamTypeBool; 
  desc[eParam_adc0_dac1].min = 0x00000000; 
  desc[eParam_adc0_dac1].max = 1;
  desc[eParam_adc0_dac1].radix = 2;

  strcpy(desc[eParam_adc0_dac2].label, "adc0_dac2");
  desc[eParam_adc0_dac2].type = eParamTypeBool; 
  desc[eParam_adc0_dac2].min = 0x00000000; 
  desc[eParam_adc0_dac2].max = 1;
  desc[eParam_adc0_dac2].radix = 2;

  strcpy(desc[eParam_adc0_dac3].label, "adc0_dac3");
  desc[eParam_adc0_dac3].type = eParamTypeBool; 
  desc[eParam_adc0_dac3].min = 0x00000000; 
  desc[eParam_adc0_dac3].max = 1;
  desc[eParam_adc0_dac3].radix = 2;

  strcpy(desc[eParam_adc1_dac0].label, "adc1_dac0");
  desc[eParam_adc1_dac0].type = eParamTypeBool; 
  desc[eParam_adc1_dac0].min = 0x00000000; 
  desc[eParam_adc1_dac0].max = 1;
  desc[eParam_adc1_dac0].radix = 2;

  strcpy(desc[eParam_adc1_dac1].label, "adc1_dac1");
  desc[eParam_adc1_dac1].type = eParamTypeBool; 
  desc[eParam_adc1_dac1].min = 0x00000000; 
  desc[eParam_adc1_dac1].max = 1;
  desc[eParam_adc1_dac1].radix = 2;

  strcpy(desc[eParam_adc1_dac2].label, "adc1_dac2");
  desc[eParam_adc1_dac2].type = eParamTypeBool; 
  desc[eParam_adc1_dac2].min = 0x00000000; 
  desc[eParam_adc1_dac2].max = 1;
  desc[eParam_adc1_dac2].radix = 2;

  strcpy(desc[eParam_adc1_dac3].label, "adc1_dac3");
  desc[eParam_adc1_dac3].type = eParamTypeBool; 
  desc[eParam_adc1_dac3].min = 0x00000000; 
  desc[eParam_adc1_dac3].max = 1;
  desc[eParam_adc1_dac3].radix = 2;

  strcpy(desc[eParam_adc2_dac0].label, "adc2_dac0");
  desc[eParam_adc2_dac0].type = eParamTypeBool; 
  desc[eParam_adc2_dac0].min = 0x00000000; 
  desc[eParam_adc2_dac0].max = 1;
  desc[eParam_adc2_dac0].radix = 2;

  strcpy(desc[eParam_adc2_dac1].label, "adc2_dac1");
  desc[eParam_adc2_dac1].type = eParamTypeBool; 
  desc[eParam_adc2_dac1].min = 0x00000000; 
  desc[eParam_adc2_dac1].max = 1;
  desc[eParam_adc2_dac1].radix = 2;

  strcpy(desc[eParam_adc2_dac2].label, "adc2_dac2");
  desc[eParam_adc2_dac2].type = eParamTypeBool; 
  desc[eParam_adc2_dac2].min = 0x00000000; 
  desc[eParam_adc2_dac2].max = 1;
  desc[eParam_adc2_dac2].radix = 2;

  strcpy(desc[eParam_adc2_dac3].label, "adc2_dac3");
  desc[eParam_adc2_dac3].type = eParamTypeBool; 
  desc[eParam_adc2_dac3].min = 0x00000000; 
  desc[eParam_adc2_dac3].max = 1;
  desc[eParam_adc2_dac3].radix = 2;

  strcpy(desc[eParam_adc3_dac0].label, "adc3_dac0");
  desc[eParam_adc3_dac0].type = eParamTypeBool; 
  desc[eParam_adc3_dac0].min = 0x00000000; 
  desc[eParam_adc3_dac0].max = 1;
  desc[eParam_adc3_dac0].radix = 2;

  strcpy(desc[eParam_adc3_dac1].label, "adc3_dac1");
  desc[eParam_adc3_dac1].type = eParamTypeBool; 
  desc[eParam_adc3_dac1].min = 0x00000000; 
  desc[eParam_adc3_dac1].max = 1;
  desc[eParam_adc3_dac1].radix = 2;

  strcpy(desc[eParam_adc3_dac2].label, "adc3_dac2");
  desc[eParam_adc3_dac2].type = eParamTypeBool; 
  desc[eParam_adc3_dac2].min = 0x00000000; 
  desc[eParam_adc3_dac2].max = 1;
  desc[eParam_adc3_dac2].radix = 2;

  strcpy(desc[eParam_adc3_dac3].label, "adc3_dac3");
  desc[eParam_adc3_dac3].type = eParamTypeBool; 
  desc[eParam_adc3_dac3].min = 0x00000000; 
  desc[eParam_adc3_dac3].max = 1;
  desc[eParam_adc3_dac3].radix = 2;

  // i/o mix
  /* strcpy(desc[eParam_adc0_dac0].label, "adc0_dac0"); */
  /* desc[eParam_adc0_dac0].type = eParamTypeAmp;  */
  /* desc[eParam_adc0_dac0].min = 0x00000000;  */
  /* desc[eParam_adc0_dac0].max = FR32_MAX; */
  /* desc[eParam_adc0_dac0].radix = 1; */

  /* strcpy(desc[eParam_adc0_dac1].label, "adc0_dac1"); */
  /* desc[eParam_adc0_dac1].type = eParamTypeAmp;  */
  /* desc[eParam_adc0_dac1].min = 0x00000000;  */
  /* desc[eParam_adc0_dac1].max = FR32_MAX; */
  /* desc[eParam_adc0_dac1].radix = 1; */

  /* strcpy(desc[eParam_adc0_dac2].label, "adc0_dac2"); */
  /* desc[eParam_adc0_dac2].type = eParamTypeAmp;  */
  /* desc[eParam_adc0_dac2].min = 0x00000000;  */
  /* desc[eParam_adc0_dac2].max = FR32_MAX; */
  /* desc[eParam_adc0_dac2].radix = 1; */

  /* strcpy(desc[eParam_adc0_dac3].label, "adc0_dac3"); */
  /* desc[eParam_adc0_dac3].type = eParamTypeAmp;  */
  /* desc[eParam_adc0_dac3].min = 0x00000000;  */
  /* desc[eParam_adc0_dac3].max = FR32_MAX; */
  /* desc[eParam_adc0_dac3].radix = 1; */

  /* strcpy(desc[eParam_adc1_dac0].label, "adc1_dac0"); */
  /* desc[eParam_adc1_dac0].type = eParamTypeAmp;  */
  /* desc[eParam_adc1_dac0].min = 0x00000000;  */
  /* desc[eParam_adc1_dac0].max = FR32_MAX; */
  /* desc[eParam_adc1_dac0].radix = 1; */

  /* strcpy(desc[eParam_adc1_dac1].label, "adc1_dac1"); */
  /* desc[eParam_adc1_dac1].type = eParamTypeAmp;  */
  /* desc[eParam_adc1_dac1].min = 0x00000000;  */
  /* desc[eParam_adc1_dac1].max = FR32_MAX; */
  /* desc[eParam_adc1_dac1].radix = 1; */

  /* strcpy(desc[eParam_adc1_dac2].label, "adc1_dac2"); */
  /* desc[eParam_adc1_dac2].type = eParamTypeAmp;  */
  /* desc[eParam_adc1_dac2].min = 0x00000000;  */
  /* desc[eParam_adc1_dac2].max = FR32_MAX; */
  /* desc[eParam_adc1_dac2].radix = 1; */

  /* strcpy(desc[eParam_adc1_dac3].label, "adc1_dac3"); */
  /* desc[eParam_adc1_dac3].type = eParamTypeAmp;  */
  /* desc[eParam_adc1_dac3].min = 0x00000000;  */
  /* desc[eParam_adc1_dac3].max = FR32_MAX; */
  /* desc[eParam_adc1_dac3].radix = 1; */

  /* strcpy(desc[eParam_adc2_dac0].label, "adc2_dac0"); */
  /* desc[eParam_adc2_dac0].type = eParamTypeAmp;  */
  /* desc[eParam_adc2_dac0].min = 0x00000000;  */
  /* desc[eParam_adc2_dac0].max = FR32_MAX; */
  /* desc[eParam_adc2_dac0].radix = 1; */

  /* strcpy(desc[eParam_adc2_dac1].label, "adc2_dac1"); */
  /* desc[eParam_adc2_dac1].type = eParamTypeAmp;  */
  /* desc[eParam_adc2_dac1].min = 0x00000000;  */
  /* desc[eParam_adc2_dac1].max = FR32_MAX; */
  /* desc[eParam_adc2_dac1].radix = 1; */

  /* strcpy(desc[eParam_adc2_dac2].label, "adc2_dac2"); */
  /* desc[eParam_adc2_dac2].type = eParamTypeAmp;  */
  /* desc[eParam_adc2_dac2].min = 0x00000000;  */
  /* desc[eParam_adc2_dac2].max = FR32_MAX; */
  /* desc[eParam_adc2_dac2].radix = 1; */

  /* strcpy(desc[eParam_adc2_dac3].label, "adc2_dac3"); */
  /* desc[eParam_adc2_dac3].type = eParamTypeAmp;  */
  /* desc[eParam_adc2_dac3].min = 0x00000000;  */
  /* desc[eParam_adc2_dac3].max = FR32_MAX; */
  /* desc[eParam_adc2_dac3].radix = 1; */

  /* strcpy(desc[eParam_adc3_dac0].label, "adc3_dac0"); */
  /* desc[eParam_adc3_dac0].type = eParamTypeAmp;  */
  /* desc[eParam_adc3_dac0].min = 0x00000000;  */
  /* desc[eParam_adc3_dac0].max = FR32_MAX; */
  /* desc[eParam_adc3_dac0].radix = 1; */

  /* strcpy(desc[eParam_adc3_dac1].label, "adc3_dac1"); */
  /* desc[eParam_adc3_dac1].type = eParamTypeAmp;  */
  /* desc[eParam_adc3_dac1].min = 0x00000000;  */
  /* desc[eParam_adc3_dac1].max = FR32_MAX; */
  /* desc[eParam_adc3_dac1].radix = 1; */

  /* strcpy(desc[eParam_adc3_dac2].label, "adc3_dac2"); */
  /* desc[eParam_adc3_dac2].type = eParamTypeAmp;  */
  /* desc[eParam_adc3_dac2].min = 0x00000000;  */
  /* desc[eParam_adc3_dac2].max = FR32_MAX; */
  /* desc[eParam_adc3_dac2].radix = 1; */

  /* strcpy(desc[eParam_adc3_dac3].label, "adc3_dac3"); */
  /* desc[eParam_adc3_dac3].type = eParamTypeAmp;  */
  /* desc[eParam_adc3_dac3].min = 0x00000000;  */
  /* desc[eParam_adc3_dac3].max = FR32_MAX; */
  /* desc[eParam_adc3_dac3].radix = 1; */

  // filter slew
  strcpy(desc[eParamCut0Slew].label, "cut0Slew");
  desc[eParamCut0Slew].type = eParamTypeIntegrator;
  desc[eParamCut0Slew].min = 0;
  desc[eParamCut0Slew].max = FR32_MAX;
  desc[eParamCut0Slew].radix = 32;

  strcpy(desc[eParamRq0Slew].label, "rq0Slew");
  desc[eParamRq0Slew].type = eParamTypeIntegrator;
  desc[eParamRq0Slew].min = 0;
  desc[eParamRq0Slew].max = FR32_MAX;
  desc[eParamRq0Slew].radix = 32;


  strcpy(desc[eParamDry0Slew].label, "dry0Slew");
  desc[eParamDry0Slew].type = eParamTypeIntegrator;
  desc[eParamDry0Slew].min = 0;
  desc[eParamDry0Slew].max = FR32_MAX;
  desc[eParamDry0Slew].radix = 32;

  strcpy(desc[eParamWet0Slew].label, "wet0Slew");
  desc[eParamWet0Slew].type = eParamTypeIntegrator;
  desc[eParamWet0Slew].min = 0;
  desc[eParamWet0Slew].max = FR32_MAX;
  desc[eParamWet0Slew].radix = 32;

  strcpy(desc[eParamCut1Slew].label, "cut1Slew");
  desc[eParamCut1Slew].type = eParamTypeIntegrator;
  desc[eParamCut1Slew].min = 0;
  desc[eParamCut1Slew].max = FR32_MAX;
  desc[eParamCut1Slew].radix = 32;

  strcpy(desc[eParamRq1Slew].label, "rq1Slew");
  desc[eParamRq1Slew].type = eParamTypeIntegrator;
  desc[eParamRq1Slew].min = 0;
  desc[eParamRq1Slew].max = FR32_MAX;
  desc[eParamRq1Slew].radix = 32;


  strcpy(desc[eParamDry1Slew].label, "dry1Slew");
  desc[eParamDry1Slew].type = eParamTypeIntegrator;
  desc[eParamDry1Slew].min = 0;
  desc[eParamDry1Slew].max = FR32_MAX;
  desc[eParamDry1Slew].radix = 32;

  strcpy(desc[eParamWet1Slew].label, "wet1Slew");
  desc[eParamWet1Slew].type = eParamTypeIntegrator;
  desc[eParamWet1Slew].min = 0;
  desc[eParamWet1Slew].max = FR32_MAX;
  desc[eParamWet1Slew].radix = 32;


}
