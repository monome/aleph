#include <string.h>

#include "module.h"
#include "params.h"

extern void fill_param_desc(ParamDesc* desc) {

  strcpy(desc[eParam_linAttack].label, "linAttack");
  desc[eParam_linAttack].type = eParamTypeFix;
  desc[eParam_linAttack].min = FR32_MAX / 48 / 1000;
  desc[eParam_linAttack].max = FR32_MAX / 48;
  desc[eParam_linAttack].radix = 16;

  strcpy(desc[eParam_linDecay].label, "linDecay");
  desc[eParam_linDecay].type = eParamTypeFix;
  desc[eParam_linDecay].min = FR32_MAX / 48 / 1000;
  desc[eParam_linDecay].max = FR32_MAX / 48;
  desc[eParam_linDecay].radix = 16;

  strcpy(desc[eParam_logAttack].label, "logAttack");
  desc[eParam_logAttack].type = eParamTypeFix;
  desc[eParam_logAttack].min = 268434;
  desc[eParam_logAttack].max = 268434000;
  desc[eParam_logAttack].radix = 16;

  strcpy(desc[eParam_logDecay].label, "logDecay");
  desc[eParam_logDecay].type = eParamTypeFix;
  desc[eParam_logDecay].min = 268434;
  desc[eParam_logDecay].max = 268434000;
  desc[eParam_logDecay].radix = 16;

  strcpy(desc[eParam_linEnv].label, "linEnv");
  desc[eParam_linEnv].type = eParamTypeFix;
  desc[eParam_linEnv].min = 0x00000000;
  desc[eParam_linEnv].max = FR32_MAX;
  desc[eParam_linEnv].radix = 16;

  strcpy(desc[eParam_logEnv].label, "logEnv");
  desc[eParam_logEnv].type = eParamTypeFix;
  desc[eParam_logEnv].min = 0x00000000;
  desc[eParam_logEnv].max = FR32_MAX;
  desc[eParam_logEnv].radix = 16;

  strcpy(desc[eParam_pitch].label, "pitch");
  desc[eParam_pitch].type = eParamTypeFix;
  desc[eParam_pitch].min = 0x00000000;
  desc[eParam_pitch].max = FR32_MAX;
  desc[eParam_pitch].radix = 16;

  strcpy(desc[eParam_cvVal3].label, "cv3");
  desc[eParam_cvVal3].type = eParamTypeFix;
  desc[eParam_cvVal3].min = 0x00000000;
  desc[eParam_cvVal3].max = PARAM_DAC_MAX;
  desc[eParam_cvVal3].radix = PARAM_DAC_RADIX;

  strcpy(desc[eParam_cvSlew3].label, "cvSlew3");
  desc[eParam_cvSlew3].type = eParamTypeIntegrator;
  desc[eParam_cvSlew3].min = 0x00000000;
  desc[eParam_cvSlew3].max = 0x7fffffff;
  desc[eParam_cvSlew3].radix = 32;
}

// EOF
