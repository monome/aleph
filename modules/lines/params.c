 
#include <string.h>
#include "module.h"
#include "param_common.h"
#include "params.h" 

void fill_param_desc(ParamDesc* desc) {
  strcpy(desc[eParam_delay0].label, "delay0");
  desc[eParam_delay0].type = eParamTypeFix; 
  desc[eParam_delay0].min = 0x00000000; 
  desc[eParam_delay0].max = PARAM_SECONDS_MAX; 
  desc[eParam_delay0].radix = PARAM_SECONDS_RADIX;

  strcpy(desc[eParam_loop0].label, "loop0");
  desc[eParam_loop0].type = eParamTypeFix; 
  desc[eParam_loop0].min = 0x00000000; 
  desc[eParam_loop0].max = PARAM_SECONDS_MAX; 
  desc[eParam_loop0].radix = PARAM_SECONDS_RADIX;

  // rate multiplier
  strcpy(desc[eParam_rMul0].label, "rMul0");
  desc[eParam_rMul0].type = eParamTypeFix; 
  desc[eParam_rMul0].min = 0x00010000; 
  desc[eParam_rMul0].max = 0x00080000; 
  desc[eParam_rMul0].radix = 32;

  // rate divider 
  strcpy(desc[eParam_rDiv0].label, "rDiv0");
  desc[eParam_rDiv0].type = eParamTypeFix; 
  desc[eParam_rDiv0].min = 0x00010000; 
  desc[eParam_rDiv0].max = 0x00080000; 
  desc[eParam_rDiv0].radix = 32;

  strcpy(desc[eParam_write0].label, "write0");
  desc[eParam_write0].type = eParamTypeAmp; 
  desc[eParam_write0].min = 0x00000000; 
  desc[eParam_write0].max = FR32_MAX;
  desc[eParam_write0].radix = 1;

  strcpy(desc[eParam_pre0].label, "pre0");
  desc[eParam_pre0].type = eParamTypeAmp; 
  desc[eParam_pre0].min = 0x00000000; 
  desc[eParam_pre0].max = FR32_MAX;
  desc[eParam_pre0].radix = 1;

  strcpy(desc[eParam_pos_write0].label, "pos_write0");
  desc[eParam_pos_write0].type = eParamTypeFix; 
  desc[eParam_pos_write0].min = 0x00000000; 
  desc[eParam_pos_write0].max = PARAM_SECONDS_MAX; 
  desc[eParam_pos_write0].radix = PARAM_SECONDS_RADIX;

  strcpy(desc[eParam_pos_read0].label, "pos_read0");
  desc[eParam_pos_read0].type = eParamTypeFix; 
  desc[eParam_pos_read0].min = 0x00000000; 
  desc[eParam_pos_read0].max = PARAM_SECONDS_MAX; 
  desc[eParam_pos_read0].radix = PARAM_SECONDS_RADIX;

  strcpy(desc[eParam_run_write0].label, "run_write0");
  desc[eParam_run_write0].type = eParamTypeBool; 
  desc[eParam_run_write0].min = 0;
  desc[eParam_run_write0].max = 1; 
  desc[eParam_run_write0].radix = 2; 

  strcpy(desc[eParam_run_read0].label, "run_read0");
  desc[eParam_run_read0].type = eParamTypeBool; 
  desc[eParam_run_read0].min = 0;
  desc[eParam_run_read0].max = 1; 
  desc[eParam_run_read0].radix = 2; 

  strcpy(desc[eParam_delay1].label, "delay1");
  desc[eParam_delay1].type = eParamTypeFix; 
  desc[eParam_delay1].min = 0x00000000; 
  desc[eParam_delay1].max = PARAM_SECONDS_MAX; 
  desc[eParam_delay1].radix = PARAM_SECONDS_RADIX;

  strcpy(desc[eParam_loop1].label, "loop1");
  desc[eParam_loop1].type = eParamTypeFix; 
  desc[eParam_loop1].min = 0x00000000; 
  desc[eParam_loop1].max = PARAM_SECONDS_MAX; 
  desc[eParam_loop1].radix = PARAM_SECONDS_RADIX;

  /// FIXME
  //// ????
  strcpy(desc[eParam_rMul1].label, "rMul1");
  desc[eParam_rMul1].type = eParamTypeFix; 
  desc[eParam_rMul1].min = 0x00010000; 
  desc[eParam_rMul1].max = 0x00080000; 
  desc[eParam_rMul1].radix = 32; 

  strcpy(desc[eParam_rDiv1].label, "rDiv1");
  desc[eParam_rDiv1].type = eParamTypeFix; 
  desc[eParam_rDiv1].min = 0x00010000; 
  desc[eParam_rDiv1].max = 0x00080000; 
  desc[eParam_rDiv1].radix = 32; 

  strcpy(desc[eParam_write1].label, "write1");
  desc[eParam_write1].type = eParamTypeAmp; 
  desc[eParam_write1].min = 0x00000000; 
  desc[eParam_write1].max = FR32_MAX;
  desc[eParam_write1].radix = 1;

  strcpy(desc[eParam_pre1].label, "pre1");
  desc[eParam_pre1].type = eParamTypeAmp; 
  desc[eParam_pre1].min = 0x00000000; 
  desc[eParam_pre1].max = FR32_MAX;
  desc[eParam_pre1].radix = 1;

  strcpy(desc[eParam_pos_write1].label, "pos_write1");
  desc[eParam_pos_write1].type = eParamTypeFix; 
  desc[eParam_pos_write1].min = 0x00000000; 
  desc[eParam_pos_write1].max = PARAM_SECONDS_MAX; 
  desc[eParam_pos_write1].radix = PARAM_SECONDS_RADIX;

  strcpy(desc[eParam_pos_read1].label, "pos_read1");
  desc[eParam_pos_read1].type = eParamTypeFix; 
  desc[eParam_pos_read1].min = 0x00000000; 
  desc[eParam_pos_read1].max = PARAM_SECONDS_MAX; 
  desc[eParam_pos_read1].radix = PARAM_SECONDS_RADIX;

  strcpy(desc[eParam_run_write1].label, "run_write1");
  desc[eParam_run_write1].type = eParamTypeBool; 
  desc[eParam_run_write1].min = 0;
  desc[eParam_run_write1].max = 1;
  desc[eParam_run_write1].radix = 2;

  strcpy(desc[eParam_run_read1].label, "run_read1");
  desc[eParam_run_read1].type = eParamTypeBool; 
  desc[eParam_run_read1].min = 0;
  desc[eParam_run_read1].max = 1;
  desc[eParam_run_read1].radix = 2;

  strcpy(desc[eParam_freq0].label, "cut0");
  desc[eParam_freq0].type = eParamTypeSvfFreq; 
  desc[eParam_freq0].min = 0x00000000; 
  desc[eParam_freq0].max = PARAM_CUT_MAX;
  desc[eParam_freq0].radix = 32;

  strcpy(desc[eParam_rq0].label, "rq0");
  desc[eParam_rq0].type = eParamTypeFix; 
  desc[eParam_rq0].min = PARAM_RQ_MIN;
  desc[eParam_rq0].max = PARAM_RQ_MAX;
  desc[eParam_rq0].radix = 2;
  
  strcpy(desc[eParam_low0].label, "low0");
  desc[eParam_low0].type = eParamTypeAmp; 
  desc[eParam_low0].min = 0x00000000; 
  desc[eParam_low0].max = FR32_MAX;
  desc[eParam_low0].radix = 1;

  strcpy(desc[eParam_high0].label, "high0");
  desc[eParam_high0].type = eParamTypeAmp; 
  desc[eParam_high0].min = 0x00000000; 
  desc[eParam_high0].max = FR32_MAX;
  desc[eParam_high0].radix = 1;

  strcpy(desc[eParam_band0].label, "band0");
  desc[eParam_band0].type = eParamTypeAmp; 
  desc[eParam_band0].min = 0x00000000; 
  desc[eParam_band0].max = FR32_MAX;
  desc[eParam_band0].radix = 1;

  strcpy(desc[eParam_notch0].label, "notch0");
  desc[eParam_notch0].type = eParamTypeAmp; 
  desc[eParam_notch0].min = 0x00000000; 
  desc[eParam_notch0].max = 0x00010000;
  desc[eParam_notch0].radix = 32;

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

  strcpy(desc[eParam_freq1].label, "cut1");
  desc[eParam_freq1].type = eParamTypeSvfFreq; 
  desc[eParam_freq1].min = 0x00000000; 
  desc[eParam_freq1].max = PARAM_CUT_MAX; 
  desc[eParam_freq1].radix = 1;

  ///// FIXME
  strcpy(desc[eParam_rq1].label, "rq1");
  desc[eParam_rq1].type = eParamTypeFix; 
  desc[eParam_rq1].min = PARAM_RQ_MIN;
  desc[eParam_rq1].max = PARAM_RQ_MAX; 
  desc[eParam_rq1].radix = 2;
 
  strcpy(desc[eParam_low1].label, "low1");
  desc[eParam_low1].type = eParamTypeAmp; 
  desc[eParam_low1].min = 0x00000000; 
  desc[eParam_low1].max = FR32_MAX;
  desc[eParam_low1].radix = 1;

  strcpy(desc[eParam_high1].label, "high1");
  desc[eParam_high1].type = eParamTypeAmp; 
  desc[eParam_high1].min = 0x00000000; 
  desc[eParam_high1].max = FR32_MAX;
  desc[eParam_high1].radix = 1;

  strcpy(desc[eParam_band1].label, "band1");
  desc[eParam_band1].type = eParamTypeAmp; 
  desc[eParam_band1].min = 0x00000000; 
  desc[eParam_band1].max = FR32_MAX;
  desc[eParam_band1].radix = 1;

  strcpy(desc[eParam_notch1].label, "notch1");
  desc[eParam_notch1].type = eParamTypeAmp; 
  desc[eParam_notch1].min = 0x00000000; 
  desc[eParam_notch1].max = FR32_MAX;
  desc[eParam_notch1].radix = 1;

  strcpy(desc[eParam_fwet1].label, "fwet1");
  desc[eParam_fwet1].type = eParamTypeAmp; 
  desc[eParam_fwet1].min = 0x00000000; 
  desc[eParam_fwet1].max = FR32_MAX; 
  desc[eParam_fwet1].radix = 1;

  strcpy(desc[eParam_fdry1].label, "fdry1");
  desc[eParam_fdry1].type = eParamTypeAmp; 
  desc[eParam_fdry1].min = 0x00000000; 
  desc[eParam_fdry1].max = FR32_MAX; 
  desc[eParam_fdry1].radix = 1;

  strcpy(desc[eParam_adc0_del0].label, "adc0_del0");
  desc[eParam_adc0_del0].type = eParamTypeAmp; 
  desc[eParam_adc0_del0].min = 0x00000000; 
  desc[eParam_adc0_del0].max = FR32_MAX;
  desc[eParam_adc0_del0].radix = 1;

  strcpy(desc[eParam_adc0_del1].label, "adc0_del1");
  desc[eParam_adc0_del1].type = eParamTypeAmp; 
  desc[eParam_adc0_del1].min = 0x00000000; 
  desc[eParam_adc0_del1].max = FR32_MAX;
  desc[eParam_adc0_del1].radix = 1;

  strcpy(desc[eParam_adc1_del0].label, "adc1_del0");
  desc[eParam_adc1_del0].type = eParamTypeAmp; 
  desc[eParam_adc1_del0].min = 0x00000000; 
  desc[eParam_adc1_del0].max = FR32_MAX;
  desc[eParam_adc1_del0].radix = 1;

  strcpy(desc[eParam_adc1_del1].label, "adc1_del1");
  desc[eParam_adc1_del1].type = eParamTypeAmp; 
  desc[eParam_adc1_del1].min = 0x00000000; 
  desc[eParam_adc1_del1].max = FR32_MAX;
  desc[eParam_adc1_del1].radix = 1;

  strcpy(desc[eParam_adc2_del0].label, "adc2_del0");
  desc[eParam_adc2_del0].type = eParamTypeAmp; 
  desc[eParam_adc2_del0].min = 0x00000000; 
  desc[eParam_adc2_del0].max = FR32_MAX;
  desc[eParam_adc2_del0].radix = 1;

  strcpy(desc[eParam_adc2_del1].label, "adc2_del1");
  desc[eParam_adc2_del1].type = eParamTypeAmp; 
  desc[eParam_adc2_del1].min = 0x00000000; 
  desc[eParam_adc2_del1].max = FR32_MAX;
  desc[eParam_adc2_del1].radix = 1;

  strcpy(desc[eParam_adc3_del0].label, "adc3_del0");
  desc[eParam_adc3_del0].type = eParamTypeAmp; 
  desc[eParam_adc3_del0].min = 0x00000000; 
  desc[eParam_adc3_del0].max = FR32_MAX;
  desc[eParam_adc3_del0].radix = 1;

  strcpy(desc[eParam_adc3_del1].label, "adc3_del1");
  desc[eParam_adc3_del1].type = eParamTypeAmp; 
  desc[eParam_adc3_del1].min = 0x00000000; 
  desc[eParam_adc3_del1].max = FR32_MAX;
  desc[eParam_adc3_del1].radix = 1;

  strcpy(desc[eParam_del0_del0].label, "del0_del0");
  desc[eParam_del0_del0].type = eParamTypeAmp; 
  desc[eParam_del0_del0].min = 0x00000000; 
  desc[eParam_del0_del0].max = FR32_MAX;
  desc[eParam_del0_del0].radix = 1;

  strcpy(desc[eParam_del0_del1].label, "del0_del1");
  desc[eParam_del0_del1].type = eParamTypeAmp; 
  desc[eParam_del0_del1].min = 0x00000000; 
  desc[eParam_del0_del1].max = FR32_MAX;
  desc[eParam_del0_del1].radix = 1;

  strcpy(desc[eParam_del1_del0].label, "del1_del0");
  desc[eParam_del1_del0].type = eParamTypeAmp; 
  desc[eParam_del1_del0].min = 0x00000000; 
  desc[eParam_del1_del0].max = FR32_MAX;
  desc[eParam_del1_del0].radix = 1;

  strcpy(desc[eParam_del1_del1].label, "del1_del1");
  desc[eParam_del1_del1].type = eParamTypeAmp; 
  desc[eParam_del1_del1].min = 0x00000000; 
  desc[eParam_del1_del1].max = FR32_MAX;
  desc[eParam_del1_del1].radix = 1;

  strcpy(desc[eParam_adc0_dac0].label, "adc0_dac0");
  desc[eParam_adc0_dac0].type = eParamTypeAmp; 
  desc[eParam_adc0_dac0].min = 0x00000000; 
  desc[eParam_adc0_dac0].max = FR32_MAX;
  desc[eParam_adc0_dac0].radix = 1;

  strcpy(desc[eParam_adc0_dac1].label, "adc0_dac1");
  desc[eParam_adc0_dac1].type = eParamTypeAmp; 
  desc[eParam_adc0_dac1].min = 0x00000000; 
  desc[eParam_adc0_dac1].max = FR32_MAX;
  desc[eParam_adc0_dac1].radix = 1;

  strcpy(desc[eParam_adc0_dac2].label, "adc0_dac2");
  desc[eParam_adc0_dac2].type = eParamTypeAmp; 
  desc[eParam_adc0_dac2].min = 0x00000000; 
  desc[eParam_adc0_dac2].max = FR32_MAX;
  desc[eParam_adc0_dac2].radix = 1;

  strcpy(desc[eParam_adc0_dac3].label, "adc0_dac3");
  desc[eParam_adc0_dac3].type = eParamTypeAmp; 
  desc[eParam_adc0_dac3].min = 0x00000000; 
  desc[eParam_adc0_dac3].max = FR32_MAX;
  desc[eParam_adc0_dac3].radix = 1;

  strcpy(desc[eParam_adc1_dac0].label, "adc1_dac0");
  desc[eParam_adc1_dac0].type = eParamTypeAmp; 
  desc[eParam_adc1_dac0].min = 0x00000000; 
  desc[eParam_adc1_dac0].max = FR32_MAX;
  desc[eParam_adc1_dac0].radix = 1;

  strcpy(desc[eParam_adc1_dac1].label, "adc1_dac1");
  desc[eParam_adc1_dac1].type = eParamTypeAmp; 
  desc[eParam_adc1_dac1].min = 0x00000000; 
  desc[eParam_adc1_dac1].max = FR32_MAX;
  desc[eParam_adc1_dac1].radix = 1;

  strcpy(desc[eParam_adc1_dac2].label, "adc1_dac2");
  desc[eParam_adc1_dac2].type = eParamTypeAmp; 
  desc[eParam_adc1_dac2].min = 0x00000000; 
  desc[eParam_adc1_dac2].max = FR32_MAX;
  desc[eParam_adc1_dac2].radix = 1;

  strcpy(desc[eParam_adc1_dac3].label, "adc1_dac3");
  desc[eParam_adc1_dac3].type = eParamTypeAmp; 
  desc[eParam_adc1_dac3].min = 0x00000000; 
  desc[eParam_adc1_dac3].max = FR32_MAX;
  desc[eParam_adc1_dac3].radix = 1;

  strcpy(desc[eParam_adc2_dac0].label, "adc2_dac0");
  desc[eParam_adc2_dac0].type = eParamTypeAmp; 
  desc[eParam_adc2_dac0].min = 0x00000000; 
  desc[eParam_adc2_dac0].max = FR32_MAX;
  desc[eParam_adc2_dac0].radix = 1;

  strcpy(desc[eParam_adc2_dac1].label, "adc2_dac1");
  desc[eParam_adc2_dac1].type = eParamTypeAmp; 
  desc[eParam_adc2_dac1].min = 0x00000000; 
  desc[eParam_adc2_dac1].max = FR32_MAX;
  desc[eParam_adc2_dac1].radix = 1;

  strcpy(desc[eParam_adc2_dac2].label, "adc2_dac2");
  desc[eParam_adc2_dac2].type = eParamTypeAmp; 
  desc[eParam_adc2_dac2].min = 0x00000000; 
  desc[eParam_adc2_dac2].max = FR32_MAX;
  desc[eParam_adc2_dac2].radix = 1;

  strcpy(desc[eParam_adc2_dac3].label, "adc2_dac3");
  desc[eParam_adc2_dac3].type = eParamTypeAmp; 
  desc[eParam_adc2_dac3].min = 0x00000000; 
  desc[eParam_adc2_dac3].max = FR32_MAX;
  desc[eParam_adc2_dac3].radix = 1;

  strcpy(desc[eParam_adc3_dac0].label, "adc3_dac0");
  desc[eParam_adc3_dac0].type = eParamTypeAmp; 
  desc[eParam_adc3_dac0].min = 0x00000000; 
  desc[eParam_adc3_dac0].max = FR32_MAX;
  desc[eParam_adc3_dac0].radix = 1;

  strcpy(desc[eParam_adc3_dac1].label, "adc3_dac1");
  desc[eParam_adc3_dac1].type = eParamTypeAmp; 
  desc[eParam_adc3_dac1].min = 0x00000000; 
  desc[eParam_adc3_dac1].max = FR32_MAX;
  desc[eParam_adc3_dac1].radix = 1;

  strcpy(desc[eParam_adc3_dac2].label, "adc3_dac2");
  desc[eParam_adc3_dac2].type = eParamTypeAmp; 
  desc[eParam_adc3_dac2].min = 0x00000000; 
  desc[eParam_adc3_dac2].max = FR32_MAX;
  desc[eParam_adc3_dac2].radix = 1;

  strcpy(desc[eParam_adc3_dac3].label, "adc3_dac3");
  desc[eParam_adc3_dac3].type = eParamTypeAmp; 
  desc[eParam_adc3_dac3].min = 0x00000000; 
  desc[eParam_adc3_dac3].max = FR32_MAX;
  desc[eParam_adc3_dac3].radix = 1;



  strcpy(desc[eParam_del0_dac0].label, "del0_dac0");
  desc[eParam_del0_dac0].type = eParamTypeAmp; 
  desc[eParam_del0_dac0].min = 0x00000000; 
  desc[eParam_del0_dac0].max = FR32_MAX;
  desc[eParam_del0_dac0].radix = 1;

  strcpy(desc[eParam_del0_dac1].label, "del0_dac1");
  desc[eParam_del0_dac1].type = eParamTypeAmp; 
  desc[eParam_del0_dac1].min = 0x00000000; 
  desc[eParam_del0_dac1].max = FR32_MAX;
  desc[eParam_del0_dac1].radix = 1;

  strcpy(desc[eParam_del0_dac2].label, "del0_dac2");
  desc[eParam_del0_dac2].type = eParamTypeAmp; 
  desc[eParam_del0_dac2].min = 0x00000000; 
  desc[eParam_del0_dac2].max = FR32_MAX;
  desc[eParam_del0_dac2].radix = 1;

  strcpy(desc[eParam_del0_dac3].label, "del0_dac3");
  desc[eParam_del0_dac3].type = eParamTypeAmp; 
  desc[eParam_del0_dac3].min = 0x00000000; 
  desc[eParam_del0_dac3].max = FR32_MAX;
  desc[eParam_del0_dac3].radix = 1;

  strcpy(desc[eParam_del1_dac0].label, "del1_dac0");
  desc[eParam_del1_dac0].type = eParamTypeAmp; 
  desc[eParam_del1_dac0].min = 0x00000000; 
  desc[eParam_del1_dac0].max = FR32_MAX;
  desc[eParam_del1_dac0].radix = 1;

  strcpy(desc[eParam_del1_dac1].label, "del1_dac1");
  desc[eParam_del1_dac1].type = eParamTypeAmp; 
  desc[eParam_del1_dac1].min = 0x00000000; 
  desc[eParam_del1_dac1].max = FR32_MAX;
  desc[eParam_del1_dac1].radix = 1;

  strcpy(desc[eParam_del1_dac2].label, "del1_dac2");
  desc[eParam_del1_dac2].type = eParamTypeAmp; 
  desc[eParam_del1_dac2].min = 0x00000000; 
  desc[eParam_del1_dac2].max = FR32_MAX;
  desc[eParam_del1_dac2].radix = 1;

  strcpy(desc[eParam_del1_dac3].label, "del1_dac3");
  desc[eParam_del1_dac3].type = eParamTypeAmp; 
  desc[eParam_del1_dac3].min = 0x00000000; 
  desc[eParam_del1_dac3].max = FR32_MAX; 
  desc[eParam_del1_dac3].radix = 1;

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

  strcpy(desc[eParamLow0Slew].label, "low0Slew");
  desc[eParamLow0Slew].type = eParamTypeIntegrator;
  desc[eParamLow0Slew].min = 0;
  desc[eParamLow0Slew].max = FR32_MAX;
  desc[eParamLow0Slew].radix = 32;

  strcpy(desc[eParamHigh0Slew].label, "high0Slew");
  desc[eParamHigh0Slew].type = eParamTypeIntegrator;
  desc[eParamHigh0Slew].min = 0;
  desc[eParamHigh0Slew].max = FR32_MAX;
  desc[eParamHigh0Slew].radix = 32;

  strcpy(desc[eParamBand0Slew].label, "band0Slew");
  desc[eParamBand0Slew].type = eParamTypeIntegrator;
  desc[eParamBand0Slew].min = 0;
  desc[eParamBand0Slew].max = FR32_MAX;
  desc[eParamBand0Slew].radix = 32;

  strcpy(desc[eParamNotch0Slew].label, "notch0Slew");
  desc[eParamNotch0Slew].type = eParamTypeIntegrator;
  desc[eParamNotch0Slew].min = 0;
  desc[eParamNotch0Slew].max = FR32_MAX;
  desc[eParamNotch0Slew].radix = 32;

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

  strcpy(desc[eParamLow1Slew].label, "low1Slew");
  desc[eParamLow1Slew].type = eParamTypeIntegrator;
  desc[eParamLow1Slew].min = 0;
  desc[eParamLow1Slew].max = FR32_MAX;
  desc[eParamLow1Slew].radix = 32;

  strcpy(desc[eParamHigh1Slew].label, "high1Slew");
  desc[eParamHigh1Slew].type = eParamTypeIntegrator;
  desc[eParamHigh1Slew].min = 0;
  desc[eParamHigh1Slew].max = FR32_MAX;
  desc[eParamHigh1Slew].radix = 32;

  strcpy(desc[eParamBand1Slew].label, "band1Slew");
  desc[eParamBand1Slew].type = eParamTypeIntegrator;
  desc[eParamBand1Slew].min = 0;
  desc[eParamBand1Slew].max = FR32_MAX;
  desc[eParamBand1Slew].radix = 32;

  strcpy(desc[eParamNotch1Slew].label, "notch1Slew");
  desc[eParamNotch1Slew].type = eParamTypeIntegrator;
  desc[eParamNotch1Slew].min = 0;
  desc[eParamNotch1Slew].max = FR32_MAX;
  desc[eParamNotch1Slew].radix = 32;

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

  strcpy(desc[eParamMixSlew].label, "mixSlew");
  desc[eParamMixSlew].type = eParamTypeIntegrator;
  desc[eParamMixSlew].min = 0;
  desc[eParamMixSlew].max = FR32_MAX;
  desc[eParamMixSlew].radix = 32;

  /////

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

  strcpy(desc[eParamFade0].label, "fade0");
  desc[eParamFade0].type = eParamTypeFix;
  desc[eParamFade0].min = PARAM_FADE_MIN;
  desc[eParamFade0].max = PARAM_FADE_MAX;
  desc[eParamFade0].radix = PARAM_FADE_RADIX;

  strcpy(desc[eParamFade1].label, "fade1");
  desc[eParamFade1].type = eParamTypeFix;
  desc[eParamFade1].min = PARAM_FADE_MIN;
  desc[eParamFade1].max = PARAM_FADE_MAX;
  desc[eParamFade1].radix = PARAM_FADE_RADIX;

}

// EOF
