#include <string.h>

#include "module.h"
#include "params.h" 

void param_desc_aux (ParamDesc* desc,int param_idx,
		     char* desc_string, ParamType type,
		     s32 min, s32 max, u8 radix) {
  strcpy(desc[param_idx].label,
	 desc_string);
  desc[param_idx].type = type;
  desc[param_idx].min = min;
  desc[param_idx].max = max;
  desc[param_idx].radix = radix;
}

extern void fill_param_desc(ParamDesc* desc) {
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

  strcpy(desc[eParam_band0Vol].label, "band0");
  desc[eParam_band0Vol].type = eParamTypeAmp;
  desc[eParam_band0Vol].min = 0x00000000;
  desc[eParam_band0Vol].max = PARAM_AMP_0;
  desc[eParam_band0Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band1Vol].label, "band1");
  desc[eParam_band1Vol].type = eParamTypeAmp;
  desc[eParam_band1Vol].min = 0x00000000;
  desc[eParam_band1Vol].max = PARAM_AMP_0;
  desc[eParam_band1Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band2Vol].label, "band2");
  desc[eParam_band2Vol].type = eParamTypeAmp;
  desc[eParam_band2Vol].min = 0x00000000;
  desc[eParam_band2Vol].max = PARAM_AMP_0;
  desc[eParam_band2Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band3Vol].label, "band3");
  desc[eParam_band3Vol].type = eParamTypeAmp;
  desc[eParam_band3Vol].min = 0x00000000;
  desc[eParam_band3Vol].max = PARAM_AMP_0;
  desc[eParam_band3Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band4Vol].label, "band4");
  desc[eParam_band4Vol].type = eParamTypeAmp;
  desc[eParam_band4Vol].min = 0x00000000;
  desc[eParam_band4Vol].max = PARAM_AMP_0;
  desc[eParam_band4Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band5Vol].label, "band5");
  desc[eParam_band5Vol].type = eParamTypeAmp;
  desc[eParam_band5Vol].min = 0x00000000;
  desc[eParam_band5Vol].max = PARAM_AMP_0;
  desc[eParam_band5Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band6Vol].label, "band6");
  desc[eParam_band6Vol].type = eParamTypeAmp;
  desc[eParam_band6Vol].min = 0x00000000;
  desc[eParam_band6Vol].max = PARAM_AMP_0;
  desc[eParam_band6Vol].radix = 16; // doesn't matter

  strcpy(desc[eParam_band7Vol].label, "band7");
  desc[eParam_band7Vol].type = eParamTypeAmp;
  desc[eParam_band7Vol].min = 0x00000000;
  desc[eParam_band7Vol].max = PARAM_AMP_0;
  desc[eParam_band7Vol].radix = 16; // doesn't matter

  param_desc_aux(desc, eParam_plosiveKick, "plosiveKick",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux(desc, eParam_plosiveDecay, "plosiveDecay",
		 eParamTypeFix, 0, 0x03E80000,
		 16);

  strcpy(desc[eParam_hissLevel].label, "Hiss");
  desc[eParam_hissLevel].type = eParamTypeAmp;
  desc[eParam_hissLevel].min = 0x00000000;
  desc[eParam_hissLevel].max = PARAM_AMP_0;
  desc[eParam_hissLevel].radix = 16; // doesn't matter

  strcpy(desc[eParam_vowelLevel].label, "Buzz");
  desc[eParam_vowelLevel].type = eParamTypeAmp;
  desc[eParam_vowelLevel].min = 0x00000000;
  desc[eParam_vowelLevel].max = PARAM_AMP_0;
  desc[eParam_vowelLevel].radix = 16; // doesn't matter

  strcpy(desc[eParam_vowelHz].label, "BuzzHz");
  desc[eParam_vowelHz].type = eParamTypeNote;
  desc[eParam_vowelHz].min = OSC_HZ_MIN;
  desc[eParam_vowelHz].max = OSC_HZ_MAX;
  desc[eParam_vowelHz].radix = OSC_HZ_RADIX;

  strcpy(desc[eParam_vowelTune].label, "BuzzTune");
  desc[eParam_vowelTune].type = eParamTypeFix;
  desc[eParam_vowelTune].min = RATIO_MIN;
  desc[eParam_vowelTune].max = RATIO_MAX;
  desc[eParam_vowelTune].radix = RATIO_RADIX;


  /* strcpy(desc[eParamTune1].label, "tune1"); */
  /* desc[eParamTune1].type = eParamTypeFix; */
  /* desc[eParamTune1].min = RATIO_MIN; */
  /* desc[eParamTune1].max = RATIO_MAX; */
  /* desc[eParamTune1].radix = RATIO_RADIX; */

}

// EOF
