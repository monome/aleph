#include <string.h>
#include "param_common.h"
#include "params.h"

void fill_param_desc(ParamDesc* desc) {
  strcpy(desc[eParamAmp0].label, "amp0");
  desc[eParamAmp0].type = eParamTypeNote;
  desc[eParamAmp0].min = 0;
  desc[eParamAmp0].max = 0x7fffffff;
  desc[eParamAmp0].radix = 1;
  
  strcpy(desc[eParamFreq0].label, "freq0");
  desc[eParamFreq0].type = eParamTypeFix;
  desc[eParamFreq0].min = 0;
  desc[eParamFreq0].max = 0x7fffffff;
  desc[eParamFreq0].radix = 1;
}
