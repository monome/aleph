#include <string.h>
#include "param_common.h"
#include "params.h"

/**
 * frequencies are expressed as raw, linear phase increment!
 * 
 * range is 0hz to nyquist! 
 *
 * coarse frequency is applied to upper 15 bits
 * fine frequency applies to lower 16 bits
 */

void fill_param_desc(ParamDesc* desc) {

  //! fill descriptor tables for avr32-readable .dsc file
  // 0
  strcpy(desc[eParamAmp0].label, "amp0");
  desc[eParamAmp0].type = eParamTypeAmp;
  desc[eParamAmp0].min = 0;
  desc[eParamAmp0].max = 0x7fffffff;
  desc[eParamAmp0].radix = 16;

  strcpy(desc[eParamFreqCoarse0].label, "freqCoarse0");
  desc[eParamFreqCoarse0].type = eParamTypeFix;
  desc[eParamFreqCoarse0].min = 0;
  desc[eParamFreqCoarse0].max = 0x7fffffff;
  desc[eParamFreqCoarse0].radix = 16;
  
  strcpy(desc[eParamFreqFine0].label, "freqFine0");
  desc[eParamFreqFine0].type = eParamTypeFix;
  desc[eParamFreqFine0].min = 0;
  desc[eParamFreqFine0].max = 0x7fffffff;
  desc[eParamFreqFine0].radix = 16;

  // 1
  strcpy(desc[eParamAmp1].label, "amp1");
  desc[eParamAmp1].type = eParamTypeAmp;
  desc[eParamAmp1].min = 0;
  desc[eParamAmp1].max = 0x7fffffff;
  desc[eParamAmp1].radix = 16;

  strcpy(desc[eParamFreqCoarse1].label, "freqCoarse1");
  desc[eParamFreqCoarse1].type = eParamTypeFix;
  desc[eParamFreqCoarse1].min = 0;
  desc[eParamFreqCoarse1].max = 0x7fffffff;
  desc[eParamFreqCoarse1].radix = 16;
  
  strcpy(desc[eParamFreqFine1].label, "freqFine1");
  desc[eParamFreqFine1].type = eParamTypeFix;
  desc[eParamFreqFine1].min = 0;
  desc[eParamFreqFine1].max = 0x7fffffff;
  desc[eParamFreqFine1].radix = 16;


  // 2
  strcpy(desc[eParamAmp2].label, "amp2");
  desc[eParamAmp2].type = eParamTypeAmp;
  desc[eParamAmp2].min = 0;
  desc[eParamAmp2].max = 0x7fffffff;
  desc[eParamAmp2].radix = 16;

  strcpy(desc[eParamFreqCoarse2].label, "freqCoarse2");
  desc[eParamFreqCoarse2].type = eParamTypeFix;
  desc[eParamFreqCoarse2].min = 0;
  desc[eParamFreqCoarse2].max = 0x7fffffff;
  desc[eParamFreqCoarse2].radix = 16;
  
  strcpy(desc[eParamFreqFine2].label, "freqFine2");
  desc[eParamFreqFine2].type = eParamTypeFix;
  desc[eParamFreqFine2].min = 0;
  desc[eParamFreqFine2].max = 0x7fffffff;
  desc[eParamFreqFine2].radix = 16;
  
  // 3
  strcpy(desc[eParamAmp3].label, "amp3");
  desc[eParamAmp3].type = eParamTypeAmp;
  desc[eParamAmp3].min = 0;
  desc[eParamAmp3].max = 0x7fffffff;
  desc[eParamAmp3].radix = 16;

  strcpy(desc[eParamFreqCoarse3].label, "freqCoarse3");
  desc[eParamFreqCoarse3].type = eParamTypeFix;
  desc[eParamFreqCoarse3].min = 0;
  desc[eParamFreqCoarse3].max = 0x7fffffff;
  desc[eParamFreqCoarse3].radix = 16;
  
  strcpy(desc[eParamFreqFine3].label, "freqFine3");
  desc[eParamFreqFine3].type = eParamTypeFix;
  desc[eParamFreqFine3].min = 0;
  desc[eParamFreqFine3].max = 0x7fffffff;
  desc[eParamFreqFine3].radix = 16;


  
}




void fill_param_text(char** txt) {
  
}
