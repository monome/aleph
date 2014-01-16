/*
  params.c

small utility to build the paramater descriptor data for a module.
easiest if a copy of this source just lives in the module directory.

compile with 
-D NAME="waves" 
, etc.

 */

#include <string.h>
#include <stdio.h>

#include "fix.h"
#include "module.h"

#include "module_custom.h"
#include "osc.h"
#include "params.h" 
#include "types.h"


//----------------
//--- static vars
ModuleData data;
ParamDesc desc[eParamNumParams];
char str[64] = "aleph-";

// big ol buffer (1meg)
#define MAXBYTES 0x100000
char buf[MAXBYTES];

//--------------------
//---- static func
void fill_param_desc(void);

////////////////////////////////
//// FIXME:
/* copying these functions from BEES.
 they are unlikely to change, but if they did it would be a pain in the ass.
 guess BEES/bfin should share pickling src (blech),
 or (at least), make this util centralized for all modules,
 so it only needs one update to sync.
*/
// pickle/unpickle for param descriptors 
// (struct definition in param_common.h
u8* pdesc_pickle(ParamDesc* pdesc, u8* dst) {
  u8 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    *dst = pdesc->label[i];
    ++dst;
  }
  // store type
  *dst = pdesc->type;
  ++dst;
  // store min
  dst = pickle_32(pdesc->min, dst);
  // store max
  dst = pickle_32(pdesc->max, dst);
  // store radix
  dst = pickle_32((u32)(pdesc->radix), dst);

  return dst;
}

/////////
////////////////////////////////

//---- main function
int main() {
  FILE* out;
  int i, j;
  // write pointer
  u8* pwr;


  data.paramDesc = desc;
  strcat(str, NAME);
  strcat(str, ".dsc");

  fill_param_desc();
  printf("\r\n filled ... \r\n");
  
  out = fopen(str, "w");

  pwr = &buf;
  // pack the count of parameters
  pwr = pickle_32(eParamNumParams, pwr);
  // serialize the descriptors
  for(i=0; i<eParamNumParams; i++) {
    pwr = pdesc_pickle(&(data.paramDesc[i]), pwr);
  }

  // write to file
  fwrite(buf, 1, (size_t)((int)pwr - (int)&buf), out);

  printf("\r\n wrote %d bytes... \r\n", ((int)pwr - (int)&buf));
  
  fclose(out);
  printf("\r\n done. \r\n");
  

  return 0;
}


void fill_param_desc(void) {

  strcpy(data.paramDesc[eParamHz0].label, "hz0");
  data.paramDesc[eParamHz0].type = eParamTypeNote;
  data.paramDesc[eParamHz0].min = OSC_HZ_MIN;
  data.paramDesc[eParamHz0].max = OSC_HZ_MAX;
  data.paramDesc[eParamHz0].radix = OSC_HZ_RADIX;
  
  strcpy(data.paramDesc[eParamHz1].label, "hz1");
  data.paramDesc[eParamHz1].type = eParamTypeNote;
  data.paramDesc[eParamHz1].min = OSC_HZ_MIN;
  data.paramDesc[eParamHz1].max = OSC_HZ_MAX;
  data.paramDesc[eParamHz1].radix = OSC_HZ_RADIX;

  strcpy(data.paramDesc[eParamTune1].label, "tune1");
  data.paramDesc[eParamTune1].type = eParamTypeFix;
  data.paramDesc[eParamTune1].min = RATIO_MIN;
  data.paramDesc[eParamTune1].max = RATIO_MAX;
  data.paramDesc[eParamTune1].radix = RATIO_RADIX;
  
  strcpy(data.paramDesc[eParamTune0].label, "tune0");
  data.paramDesc[eParamTune0].type = eParamTypeFix;
  data.paramDesc[eParamTune0].min = RATIO_MIN;
  data.paramDesc[eParamTune0].max = RATIO_MAX;
  data.paramDesc[eParamTune0].radix = RATIO_RADIX;
    
  strcpy(data.paramDesc[eParamAmp1].label, "amp1");
  data.paramDesc[eParamAmp1].type = eParamTypeAmp;
  data.paramDesc[eParamAmp1].min = 0;
  data.paramDesc[eParamAmp1].max = FRACT32_MAX >> 1;
  data.paramDesc[eParamAmp1].radix = 32;
  
  strcpy(data.paramDesc[eParamAmp0].label, "amp0");
  data.paramDesc[eParamAmp0].type = eParamTypeAmp;
  data.paramDesc[eParamAmp0].min = 0;
  data.paramDesc[eParamAmp0].max = FRACT32_MAX >> 1;
  data.paramDesc[eParamAmp0].radix = 30;
  
  strcpy(data.paramDesc[eParamPm10].label, "pm10");
  data.paramDesc[eParamPm10].type = eParamTypeFix;
  data.paramDesc[eParamPm10].min = 0;
  data.paramDesc[eParamPm10].max = FRACT32_MAX;
  data.paramDesc[eParamPm10].radix = 1;

  strcpy(data.paramDesc[eParamPm01].label, "pm01");
  data.paramDesc[eParamPm01].type = eParamTypeFix;
  data.paramDesc[eParamPm01].min = 0;
  data.paramDesc[eParamPm01].max = FRACT32_MAX;
  data.paramDesc[eParamPm01].radix = 1;

  strcpy(data.paramDesc[eParamWm10].label, "wm10");
  data.paramDesc[eParamWm10].type = eParamTypeFix;
  data.paramDesc[eParamWm10].min = 0;
  data.paramDesc[eParamWm10].max = FRACT32_MAX;
  data.paramDesc[eParamWm10].radix = 1;

  strcpy(data.paramDesc[eParamWm01].label, "wm01");
  data.paramDesc[eParamWm01].type = eParamTypeFix;
  data.paramDesc[eParamWm01].min = 0;
  data.paramDesc[eParamWm01].max = FRACT32_MAX;
  data.paramDesc[eParamWm01].radix = 1;
  
  strcpy(data.paramDesc[eParamWave1].label, "wave1");
  data.paramDesc[eParamWave1].type = eParamTypeFix;
  data.paramDesc[eParamWave1].min = 0;
  data.paramDesc[eParamWave1].max = FRACT32_MAX;
  data.paramDesc[eParamWave1].radix = 1;
  
  strcpy(data.paramDesc[eParamWave0].label, "wave0");
  data.paramDesc[eParamWave0].type = eParamTypeFix;
  data.paramDesc[eParamWave0].min = 0;
  data.paramDesc[eParamWave0].max = FRACT32_MAX;
  data.paramDesc[eParamWave0].radix = 1;
 
  strcpy(data.paramDesc[eParamBl1].label, "bl1");
  data.paramDesc[eParamBl1].type = eParamTypeFix;
  data.paramDesc[eParamBl1].min = 0;
  data.paramDesc[eParamBl1].max = FRACT32_MAX;
  data.paramDesc[eParamBl1].radix = 1;
  
  strcpy(data.paramDesc[eParamBl0].label, "bl0");
  data.paramDesc[eParamBl0].type = eParamTypeFix;
  data.paramDesc[eParamBl0].min = 0;
  data.paramDesc[eParamBl0].max = FRACT32_MAX;
  data.paramDesc[eParamBl0].radix = 1;

  /////////////////////
  /////////////

  // svf 1
  strcpy(data.paramDesc[eParam_cut1].label, "cut1");
  data.paramDesc[eParam_cut1].type = eParamTypeSvfFreq; 
  data.paramDesc[eParam_cut1].min = 0x00000000; 
  data.paramDesc[eParam_cut1].max = PARAM_CUT_MAX;
  data.paramDesc[eParam_cut1].radix = 32;

  ///// FIXME
  strcpy(data.paramDesc[eParam_rq1].label, "rq1");
  data.paramDesc[eParam_rq1].type = eParamTypeFix; 
  data.paramDesc[eParam_rq1].min = PARAM_RQ_MIN;
  data.paramDesc[eParam_rq1].max = PARAM_RQ_MAX;
  data.paramDesc[eParam_rq1].radix = 2;
  
  strcpy(data.paramDesc[eParam_low1].label, "low1");
  data.paramDesc[eParam_low1].type = eParamTypeAmp; 
  data.paramDesc[eParam_low1].min = 0x00000000; 
  data.paramDesc[eParam_low1].max = FR32_MAX;
  data.paramDesc[eParam_low1].radix = 0;

  strcpy(data.paramDesc[eParam_high1].label, "high1");
  data.paramDesc[eParam_high1].type = eParamTypeAmp; 
  data.paramDesc[eParam_high1].min = 0x00000000; 
  data.paramDesc[eParam_high1].max = FR32_MAX;
  data.paramDesc[eParam_high1].radix = 0;

  strcpy(data.paramDesc[eParam_band1].label, "band1");
  data.paramDesc[eParam_band1].type = eParamTypeAmp; 
  data.paramDesc[eParam_band1].min = 0x00000000; 
  data.paramDesc[eParam_band1].max = FR32_MAX;
  data.paramDesc[eParam_band1].radix = 0;

  strcpy(data.paramDesc[eParam_notch1].label, "notch1");
  data.paramDesc[eParam_notch1].type = eParamTypeAmp; 
  data.paramDesc[eParam_notch1].min = 0x00000000; 
  data.paramDesc[eParam_notch1].max = FR32_MAX;
  data.paramDesc[eParam_notch1].radix = 0;

  strcpy(data.paramDesc[eParam_fwet1].label, "fwet1");
  data.paramDesc[eParam_fwet1].type = eParamTypeAmp; 
  data.paramDesc[eParam_fwet1].min = 0x00000000; 
  data.paramDesc[eParam_fwet1].max = FR32_MAX; 
  data.paramDesc[eParam_fwet1].radix = 0;

  strcpy(data.paramDesc[eParam_fdry1].label, "fdry1");
  data.paramDesc[eParam_fdry1].type = eParamTypeAmp; 
  data.paramDesc[eParam_fdry1].min = 0x00000000; 
  data.paramDesc[eParam_fdry1].max = FR32_MAX; 
  data.paramDesc[eParam_fdry1].radix = 0;

  // svf 0
  strcpy(data.paramDesc[eParam_cut0].label, "cut0");
  data.paramDesc[eParam_cut0].type = eParamTypeSvfFreq; 
  data.paramDesc[eParam_cut0].min = 0x00000000; 
  data.paramDesc[eParam_cut0].max = PARAM_CUT_MAX;
  data.paramDesc[eParam_cut0].radix = 32;

  ///// FIXME
  strcpy(data.paramDesc[eParam_rq0].label, "rq0");
  data.paramDesc[eParam_rq0].type = eParamTypeFix; 
  data.paramDesc[eParam_rq0].min = PARAM_RQ_MIN;
  data.paramDesc[eParam_rq0].max = PARAM_RQ_MAX;
  data.paramDesc[eParam_rq0].radix = 2;
  
  strcpy(data.paramDesc[eParam_low0].label, "low0");
  data.paramDesc[eParam_low0].type = eParamTypeAmp; 
  data.paramDesc[eParam_low0].min = 0x00000000; 
  data.paramDesc[eParam_low0].max = FR32_MAX;
  data.paramDesc[eParam_low0].radix = 1;

  strcpy(data.paramDesc[eParam_high0].label, "high0");
  data.paramDesc[eParam_high0].type = eParamTypeAmp; 
  data.paramDesc[eParam_high0].min = 0x00000000; 
  data.paramDesc[eParam_high0].max = FR32_MAX;
  data.paramDesc[eParam_high0].radix = 1;

  strcpy(data.paramDesc[eParam_band0].label, "band0");
  data.paramDesc[eParam_band0].type = eParamTypeAmp; 
  data.paramDesc[eParam_band0].min = 0x00000000; 
  data.paramDesc[eParam_band0].max = FR32_MAX;
  data.paramDesc[eParam_band0].radix = 1;

  strcpy(data.paramDesc[eParam_notch0].label, "notch0");
  data.paramDesc[eParam_notch0].type = eParamTypeAmp; 
  data.paramDesc[eParam_notch0].min = 0x00000000; 
  data.paramDesc[eParam_notch0].max = FR32_MAX;
  data.paramDesc[eParam_notch0].radix = 1;

  strcpy(data.paramDesc[eParam_fwet0].label, "fwet0");
  data.paramDesc[eParam_fwet0].type = eParamTypeAmp; 
  data.paramDesc[eParam_fwet0].min = 0x00000000; 
  data.paramDesc[eParam_fwet0].max = FR32_MAX; 
  data.paramDesc[eParam_fwet0].radix = 1;

  strcpy(data.paramDesc[eParam_fdry0].label, "fdry0");
  data.paramDesc[eParam_fdry0].type = eParamTypeAmp; 
  data.paramDesc[eParam_fdry0].min = 0x00000000; 
  data.paramDesc[eParam_fdry0].max = FR32_MAX; 
  data.paramDesc[eParam_fdry0].radix = 1;

  ////////
  //////

  strcpy(data.paramDesc[eParamHz0Slew].label, "hz0Slew");
  data.paramDesc[eParamHz0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamHz0Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamHz0Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamHz0Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(data.paramDesc[eParamHz1Slew].label, "hz1Slew");
  data.paramDesc[eParamHz1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamHz1Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamHz1Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamHz1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(data.paramDesc[eParamPm10Slew].label, "pm10Slew");
  data.paramDesc[eParamPm10Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamPm10Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamPm10Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamPm10Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(data.paramDesc[eParamPm01Slew].label, "pm01Slew");
  data.paramDesc[eParamPm01Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamPm01Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamPm01Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamPm01Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(data.paramDesc[eParamWm10Slew].label, "wm10Slew");
  data.paramDesc[eParamWm10Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamWm10Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamWm10Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamWm10Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(data.paramDesc[eParamWm01Slew].label, "wm01Slew");
  data.paramDesc[eParamWm01Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamWm01Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamWm01Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamWm01Slew].radix = SLEW_SECONDS_RADIX;

  strcpy(data.paramDesc[eParamWave1Slew].label, "wave1Slew");
  data.paramDesc[eParamWave1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamWave1Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamWave1Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamWave1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(data.paramDesc[eParamWave0Slew].label, "wave0Slew");
  data.paramDesc[eParamWave0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamWave0Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamWave0Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamWave0Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(data.paramDesc[eParamAmp1Slew].label, "amp1Slew");
  data.paramDesc[eParamAmp1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamAmp1Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamAmp1Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamAmp1Slew].radix = SLEW_SECONDS_RADIX;
  
  strcpy(data.paramDesc[eParamAmp0Slew].label, "amp0Slew");
  data.paramDesc[eParamAmp0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamAmp0Slew].min = SLEW_SECONDS_MIN;
  data.paramDesc[eParamAmp0Slew].max = SLEW_SECONDS_MAX;
  data.paramDesc[eParamAmp0Slew].radix = SLEW_SECONDS_RADIX;

  /* strcpy(data.paramDesc[eParamIoAmp0].label, "ioAmp1"); */
  /* data.paramDesc[eParamIoAmp0].type = eParamTypeAmp; */
  /* data.paramDesc[eParamIoAmp0].min = 0; */
  /* data.paramDesc[eParamIoAmp0].max = FRACT32_MAX >> 1; */
  /* data.paramDesc[eParamIoAmp0].radix = 32; */

  /* strcpy(data.paramDesc[eParamIoAmp1].label, "ioAmp2"); */
  /* data.paramDesc[eParamIoAmp1].type = eParamTypeAmp; */
  /* data.paramDesc[eParamIoAmp1].min = 0; */
  /* data.paramDesc[eParamIoAmp1].max = FRACT32_MAX >> 1; */
  /* data.paramDesc[eParamIoAmp1].radix = 32; */

  /* strcpy(data.paramDesc[eParamIoAmp2].label, "ioAmp3"); */
  /* data.paramDesc[eParamIoAmp2].type = eParamTypeAmp; */
  /* data.paramDesc[eParamIoAmp2].min = 0; */
  /* data.paramDesc[eParamIoAmp2].max = FRACT32_MAX >> 1; */
  /* data.paramDesc[eParamIoAmp2].radix = 32; */

  /* strcpy(data.paramDesc[eParamIoAmp3].label, "ioAmp4"); */
  /* data.paramDesc[eParamIoAmp3].type = eParamTypeAmp; */
  /* data.paramDesc[eParamIoAmp3].min = 0; */
  /* data.paramDesc[eParamIoAmp3].max = FRACT32_MAX >> 1; */
  /* data.paramDesc[eParamIoAmp3].radix = 32; */


  strcpy(data.paramDesc[eParam_cvVal0].label, "cv0");
  data.paramDesc[eParam_cvVal0].type = eParamTypeFix; 
  data.paramDesc[eParam_cvVal0].min = 0x00000000; 
  data.paramDesc[eParam_cvVal0].max = PARAM_DAC_MAX;
  data.paramDesc[eParam_cvVal0].radix = PARAM_DAC_RADIX;

  strcpy(data.paramDesc[eParam_cvVal1].label, "cv1");
  data.paramDesc[eParam_cvVal1].type = eParamTypeFix; 
  data.paramDesc[eParam_cvVal1].min = 0x00000000; 
  data.paramDesc[eParam_cvVal1].max = PARAM_DAC_MAX;
  data.paramDesc[eParam_cvVal1].radix = PARAM_DAC_RADIX;

  strcpy(data.paramDesc[eParam_cvVal2].label, "cv2");
  data.paramDesc[eParam_cvVal2].type = eParamTypeFix; 
  data.paramDesc[eParam_cvVal2].min = 0x00000000; 
  data.paramDesc[eParam_cvVal2].max = PARAM_DAC_MAX;
  data.paramDesc[eParam_cvVal2].radix = PARAM_DAC_RADIX;

  strcpy(data.paramDesc[eParam_cvVal3].label, "cv3");
  data.paramDesc[eParam_cvVal3].type = eParamTypeFix; 
  data.paramDesc[eParam_cvVal3].min = 0x00000000; 
  data.paramDesc[eParam_cvVal3].max = PARAM_DAC_MAX;
  data.paramDesc[eParam_cvVal3].radix = PARAM_DAC_RADIX;

  strcpy(data.paramDesc[eParam_cvSlew0].label, "cvSlew0");
  data.paramDesc[eParam_cvSlew0].type = eParamTypeIntegrator; 
  data.paramDesc[eParam_cvSlew0].min = 0x00000000; 
  data.paramDesc[eParam_cvSlew0].max = 0x7fffffff;
  data.paramDesc[eParam_cvSlew0].radix = 32;

  strcpy(data.paramDesc[eParam_cvSlew1].label, "cvSlew1");
  data.paramDesc[eParam_cvSlew1].type = eParamTypeIntegrator; 
  data.paramDesc[eParam_cvSlew1].min = 0x00000000; 
  data.paramDesc[eParam_cvSlew1].max = 0x7fffffff;
  data.paramDesc[eParam_cvSlew1].radix = 32;

  strcpy(data.paramDesc[eParam_cvSlew2].label, "cvSlew2");
  data.paramDesc[eParam_cvSlew2].type = eParamTypeIntegrator; 
  data.paramDesc[eParam_cvSlew2].min = 0x00000000; 
  data.paramDesc[eParam_cvSlew2].max = 0x7fffffff; 
  data.paramDesc[eParam_cvSlew2].radix = 32;

  strcpy(data.paramDesc[eParam_cvSlew3].label, "cvSlew3");
  data.paramDesc[eParam_cvSlew3].type = eParamTypeIntegrator; 
  data.paramDesc[eParam_cvSlew3].min = 0x00000000; 
  data.paramDesc[eParam_cvSlew3].max = 0x7fffffff;
  data.paramDesc[eParam_cvSlew3].radix = 32;

  // osc mix

  strcpy(data.paramDesc[eParam_osc0_dac0].label, "osc0_dac0");
  data.paramDesc[eParam_osc0_dac0].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc0_dac0].min = 0x00000000; 
  data.paramDesc[eParam_osc0_dac0].max = FR32_MAX;
  data.paramDesc[eParam_osc0_dac0].radix = 1;

  strcpy(data.paramDesc[eParam_osc0_dac1].label, "osc0_dac1");
  data.paramDesc[eParam_osc0_dac1].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc0_dac1].min = 0x00000000; 
  data.paramDesc[eParam_osc0_dac1].max = FR32_MAX;
  data.paramDesc[eParam_osc0_dac1].radix = 1;

  strcpy(data.paramDesc[eParam_osc0_dac2].label, "osc0_dac2");
  data.paramDesc[eParam_osc0_dac2].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc0_dac2].min = 0x00000000; 
  data.paramDesc[eParam_osc0_dac2].max = FR32_MAX;
  data.paramDesc[eParam_osc0_dac2].radix = 1;

  strcpy(data.paramDesc[eParam_osc0_dac3].label, "osc0_dac3");
  data.paramDesc[eParam_osc0_dac3].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc0_dac3].min = 0x00000000; 
  data.paramDesc[eParam_osc0_dac3].max = FR32_MAX;
  data.paramDesc[eParam_osc0_dac3].radix = 1;

  strcpy(data.paramDesc[eParam_osc1_dac0].label, "osc1_dac0");
  data.paramDesc[eParam_osc1_dac0].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc1_dac0].min = 0x00000000; 
  data.paramDesc[eParam_osc1_dac0].max = FR32_MAX;
  data.paramDesc[eParam_osc1_dac0].radix = 1;

  strcpy(data.paramDesc[eParam_osc1_dac1].label, "osc1_dac1");
  data.paramDesc[eParam_osc1_dac1].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc1_dac1].min = 0x00000000; 
  data.paramDesc[eParam_osc1_dac1].max = FR32_MAX;
  data.paramDesc[eParam_osc1_dac1].radix = 1;

  strcpy(data.paramDesc[eParam_osc1_dac2].label, "osc1_dac2");
  data.paramDesc[eParam_osc1_dac2].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc1_dac2].min = 0x00000000; 
  data.paramDesc[eParam_osc1_dac2].max = FR32_MAX;
  data.paramDesc[eParam_osc1_dac2].radix = 1;

  strcpy(data.paramDesc[eParam_osc1_dac3].label, "osc1_dac3");
  data.paramDesc[eParam_osc1_dac3].type = eParamTypeAmp; 
  data.paramDesc[eParam_osc1_dac3].min = 0x00000000; 
  data.paramDesc[eParam_osc1_dac3].max = FR32_MAX; 
  data.paramDesc[eParam_osc1_dac3].radix = 1;

  // i/o mix
  strcpy(data.paramDesc[eParam_adc0_dac0].label, "adc0_dac0");
  data.paramDesc[eParam_adc0_dac0].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc0_dac0].min = 0x00000000; 
  data.paramDesc[eParam_adc0_dac0].max = FR32_MAX;
  data.paramDesc[eParam_adc0_dac0].radix = 1;

  strcpy(data.paramDesc[eParam_adc0_dac1].label, "adc0_dac1");
  data.paramDesc[eParam_adc0_dac1].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc0_dac1].min = 0x00000000; 
  data.paramDesc[eParam_adc0_dac1].max = FR32_MAX;
  data.paramDesc[eParam_adc0_dac1].radix = 1;

  strcpy(data.paramDesc[eParam_adc0_dac2].label, "adc0_dac2");
  data.paramDesc[eParam_adc0_dac2].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc0_dac2].min = 0x00000000; 
  data.paramDesc[eParam_adc0_dac2].max = FR32_MAX;
  data.paramDesc[eParam_adc0_dac2].radix = 1;

  strcpy(data.paramDesc[eParam_adc0_dac3].label, "adc0_dac3");
  data.paramDesc[eParam_adc0_dac3].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc0_dac3].min = 0x00000000; 
  data.paramDesc[eParam_adc0_dac3].max = FR32_MAX;
  data.paramDesc[eParam_adc0_dac3].radix = 1;

  strcpy(data.paramDesc[eParam_adc1_dac0].label, "adc1_dac0");
  data.paramDesc[eParam_adc1_dac0].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc1_dac0].min = 0x00000000; 
  data.paramDesc[eParam_adc1_dac0].max = FR32_MAX;
  data.paramDesc[eParam_adc1_dac0].radix = 1;

  strcpy(data.paramDesc[eParam_adc1_dac1].label, "adc1_dac1");
  data.paramDesc[eParam_adc1_dac1].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc1_dac1].min = 0x00000000; 
  data.paramDesc[eParam_adc1_dac1].max = FR32_MAX;
  data.paramDesc[eParam_adc1_dac1].radix = 1;

  strcpy(data.paramDesc[eParam_adc1_dac2].label, "adc1_dac2");
  data.paramDesc[eParam_adc1_dac2].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc1_dac2].min = 0x00000000; 
  data.paramDesc[eParam_adc1_dac2].max = FR32_MAX;
  data.paramDesc[eParam_adc1_dac2].radix = 1;

  strcpy(data.paramDesc[eParam_adc1_dac3].label, "adc1_dac3");
  data.paramDesc[eParam_adc1_dac3].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc1_dac3].min = 0x00000000; 
  data.paramDesc[eParam_adc1_dac3].max = FR32_MAX;
  data.paramDesc[eParam_adc1_dac3].radix = 1;

  strcpy(data.paramDesc[eParam_adc2_dac0].label, "adc2_dac0");
  data.paramDesc[eParam_adc2_dac0].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc2_dac0].min = 0x00000000; 
  data.paramDesc[eParam_adc2_dac0].max = FR32_MAX;
  data.paramDesc[eParam_adc2_dac0].radix = 1;

  strcpy(data.paramDesc[eParam_adc2_dac1].label, "adc2_dac1");
  data.paramDesc[eParam_adc2_dac1].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc2_dac1].min = 0x00000000; 
  data.paramDesc[eParam_adc2_dac1].max = FR32_MAX;
  data.paramDesc[eParam_adc2_dac1].radix = 1;

  strcpy(data.paramDesc[eParam_adc2_dac2].label, "adc2_dac2");
  data.paramDesc[eParam_adc2_dac2].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc2_dac2].min = 0x00000000; 
  data.paramDesc[eParam_adc2_dac2].max = FR32_MAX;
  data.paramDesc[eParam_adc2_dac2].radix = 1;

  strcpy(data.paramDesc[eParam_adc2_dac3].label, "adc2_dac3");
  data.paramDesc[eParam_adc2_dac3].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc2_dac3].min = 0x00000000; 
  data.paramDesc[eParam_adc2_dac3].max = FR32_MAX;
  data.paramDesc[eParam_adc2_dac3].radix = 1;

  strcpy(data.paramDesc[eParam_adc3_dac0].label, "adc3_dac0");
  data.paramDesc[eParam_adc3_dac0].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc3_dac0].min = 0x00000000; 
  data.paramDesc[eParam_adc3_dac0].max = FR32_MAX;
  data.paramDesc[eParam_adc3_dac0].radix = 1;

  strcpy(data.paramDesc[eParam_adc3_dac1].label, "adc3_dac1");
  data.paramDesc[eParam_adc3_dac1].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc3_dac1].min = 0x00000000; 
  data.paramDesc[eParam_adc3_dac1].max = FR32_MAX;
  data.paramDesc[eParam_adc3_dac1].radix = 1;

  strcpy(data.paramDesc[eParam_adc3_dac2].label, "adc3_dac2");
  data.paramDesc[eParam_adc3_dac2].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc3_dac2].min = 0x00000000; 
  data.paramDesc[eParam_adc3_dac2].max = FR32_MAX;
  data.paramDesc[eParam_adc3_dac2].radix = 1;

  strcpy(data.paramDesc[eParam_adc3_dac3].label, "adc3_dac3");
  data.paramDesc[eParam_adc3_dac3].type = eParamTypeAmp; 
  data.paramDesc[eParam_adc3_dac3].min = 0x00000000; 
  data.paramDesc[eParam_adc3_dac3].max = FR32_MAX;
  data.paramDesc[eParam_adc3_dac3].radix = 1;


  // filter slew

  strcpy(data.paramDesc[eParamCut0Slew].label, "cut0Slew");
  data.paramDesc[eParamCut0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamCut0Slew].min = 0;
  data.paramDesc[eParamCut0Slew].max = FR32_MAX;
  data.paramDesc[eParamCut0Slew].radix = 32;

  strcpy(data.paramDesc[eParamRq0Slew].label, "rq0Slew");
  data.paramDesc[eParamRq0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamRq0Slew].min = 0;
  data.paramDesc[eParamRq0Slew].max = FR32_MAX;
  data.paramDesc[eParamRq0Slew].radix = 32;

  strcpy(data.paramDesc[eParamLow0Slew].label, "low0Slew");
  data.paramDesc[eParamLow0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamLow0Slew].min = 0;
  data.paramDesc[eParamLow0Slew].max = FR32_MAX;
  data.paramDesc[eParamLow0Slew].radix = 32;

  strcpy(data.paramDesc[eParamHigh0Slew].label, "high0Slew");
  data.paramDesc[eParamHigh0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamHigh0Slew].min = 0;
  data.paramDesc[eParamHigh0Slew].max = FR32_MAX;
  data.paramDesc[eParamHigh0Slew].radix = 32;

  strcpy(data.paramDesc[eParamBand0Slew].label, "band0Slew");
  data.paramDesc[eParamBand0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamBand0Slew].min = 0;
  data.paramDesc[eParamBand0Slew].max = FR32_MAX;
  data.paramDesc[eParamBand0Slew].radix = 32;

  strcpy(data.paramDesc[eParamNotch0Slew].label, "notch0Slew");
  data.paramDesc[eParamNotch0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamNotch0Slew].min = 0;
  data.paramDesc[eParamNotch0Slew].max = FR32_MAX;
  data.paramDesc[eParamNotch0Slew].radix = 32;

  strcpy(data.paramDesc[eParamDry0Slew].label, "dry0Slew");
  data.paramDesc[eParamDry0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamDry0Slew].min = 0;
  data.paramDesc[eParamDry0Slew].max = FR32_MAX;
  data.paramDesc[eParamDry0Slew].radix = 32;

  strcpy(data.paramDesc[eParamWet0Slew].label, "wet0Slew");
  data.paramDesc[eParamWet0Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamWet0Slew].min = 0;
  data.paramDesc[eParamWet0Slew].max = FR32_MAX;
  data.paramDesc[eParamWet0Slew].radix = 32;

  strcpy(data.paramDesc[eParamCut1Slew].label, "cut1Slew");
  data.paramDesc[eParamCut1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamCut1Slew].min = 0;
  data.paramDesc[eParamCut1Slew].max = FR32_MAX;
  data.paramDesc[eParamCut1Slew].radix = 32;

  strcpy(data.paramDesc[eParamRq1Slew].label, "rq1Slew");
  data.paramDesc[eParamRq1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamRq1Slew].min = 0;
  data.paramDesc[eParamRq1Slew].max = FR32_MAX;
  data.paramDesc[eParamRq1Slew].radix = 32;

  strcpy(data.paramDesc[eParamLow1Slew].label, "low1Slew");
  data.paramDesc[eParamLow1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamLow1Slew].min = 0;
  data.paramDesc[eParamLow1Slew].max = FR32_MAX;
  data.paramDesc[eParamLow1Slew].radix = 32;

  strcpy(data.paramDesc[eParamHigh1Slew].label, "high1Slew");
  data.paramDesc[eParamHigh1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamHigh1Slew].min = 0;
  data.paramDesc[eParamHigh1Slew].max = FR32_MAX;
  data.paramDesc[eParamHigh1Slew].radix = 32;

  strcpy(data.paramDesc[eParamBand1Slew].label, "band1Slew");
  data.paramDesc[eParamBand1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamBand1Slew].min = 0;
  data.paramDesc[eParamBand1Slew].max = FR32_MAX;
  data.paramDesc[eParamBand1Slew].radix = 32;

  strcpy(data.paramDesc[eParamNotch1Slew].label, "notch1Slew");
  data.paramDesc[eParamNotch1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamNotch1Slew].min = 0;
  data.paramDesc[eParamNotch1Slew].max = FR32_MAX;
  data.paramDesc[eParamNotch1Slew].radix = 32;

  strcpy(data.paramDesc[eParamDry1Slew].label, "dry1Slew");
  data.paramDesc[eParamDry1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamDry1Slew].min = 0;
  data.paramDesc[eParamDry1Slew].max = FR32_MAX;
  data.paramDesc[eParamDry1Slew].radix = 32;

  strcpy(data.paramDesc[eParamWet1Slew].label, "wet1Slew");
  data.paramDesc[eParamWet1Slew].type = eParamTypeIntegrator;
  data.paramDesc[eParamWet1Slew].min = 0;
  data.paramDesc[eParamWet1Slew].max = FR32_MAX;
  data.paramDesc[eParamWet1Slew].radix = 32;


  strcpy(data.paramDesc[eParamMixSlew].label, "mixSlew");
  data.paramDesc[eParamMixSlew].type = eParamTypeIntegrator;
  data.paramDesc[eParamMixSlew].min = 0;
  data.paramDesc[eParamMixSlew].max = FR32_MAX;
  data.paramDesc[eParamMixSlew].radix = 32;

}
