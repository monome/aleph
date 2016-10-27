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

  param_desc_aux(desc, eParam_noteTrigger, "buzzTrigger",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);

  strcpy(desc[eParam_noteHz].label, "BuzzHz");
  desc[eParam_noteHz].type = eParamTypeNote;
  desc[eParam_noteHz].min = OSC_HZ_MIN;
  desc[eParam_noteHz].max = OSC_HZ_MAX;
  desc[eParam_noteHz].radix = OSC_HZ_RADIX;

  strcpy(desc[eParam_noteTune].label, "BuzzTune");
  desc[eParam_noteTune].type = eParamTypeFix;
  desc[eParam_noteTune].min = RATIO_MIN;
  desc[eParam_noteTune].max = RATIO_MAX;
  desc[eParam_noteTune].radix = RATIO_RADIX;

  strcpy(desc[eParam_noteLevel].label, "Buzz");
  desc[eParam_noteLevel].type = eParamTypeAmp;
  desc[eParam_noteLevel].min = 0x00000000;
  desc[eParam_noteLevel].max = PARAM_AMP_0;
  desc[eParam_noteLevel].radix = 16; // doesn't matter
 
}

// EOF
