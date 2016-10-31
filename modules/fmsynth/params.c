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

#define param_desc_fader(param_idx, desc_string)  param_desc_aux (desc, param_idx, desc_string, FADER_TYPE, FADER_MIN, FADER_MAX, FADER_RADIX)
#define param_desc_pan(param_idx, desc_string)  param_desc_aux (desc, param_idx, desc_string, PAN_TYPE, PAN_MIN, PAN_MAX, PAN_RADIX)
#define param_desc_ratio(param_idx, desc_string)  param_desc_aux (desc, param_idx, desc_string, eParamTypeFix, RATIO_MIN, RATIO_MAX, RATIO_RADIX)
#define param_desc_adsrTime(param_idx, desc_string)  param_desc_aux(desc, param_idx, desc_string, eParamTypeFix, 0x00010000, 0x00960000, 32);

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


  param_desc_fader(eParam_op1Vol, "op1Vol");
  param_desc_pan(eParam_op1Pan, "op1Pan");

  param_desc_fader(eParam_op2Vol, "op2Vol");
  param_desc_pan(eParam_op2Pan, "op2Pan");

  param_desc_fader(eParam_op3Vol, "op3Vol");
  param_desc_pan(eParam_op3Pan, "op3Pan");

  param_desc_fader(eParam_op4Vol, "op4Vol");
  param_desc_pan(eParam_op4Pan, "op4Pan");

  param_desc_aux(desc, eParam_noteHz, "noteHz",
		 eParamTypeNote, OSC_HZ_MIN, OSC_HZ_MAX,
		 OSC_HZ_RADIX);
  param_desc_ratio(eParam_noteTune, "noteTune");
  param_desc_aux(desc, eParam_noteTrigger, "noteTrigger",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_fader(eParam_noteVelocity, "noteVelocity");

  param_desc_aux(desc, eParam_op1Mod1Source, "op1Mod1Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op1Mod1Gain, "op1Mod1Gain");
  param_desc_aux(desc, eParam_op1Mod2Source, "op1Mod2Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op1Mod2Gain, "op1Mod2Gain");
  param_desc_ratio(eParam_op1Tune, "op1Tune");
  param_desc_adsrTime(eParam_op1Attack, "op1Attack");
  param_desc_adsrTime(eParam_op1Decay, "op1Decay");
  param_desc_fader(eParam_op1Sustain, "op1Sustain");
  param_desc_adsrTime(eParam_op1Release, "op1Release");
  param_desc_aux(desc, eParam_op1Waveshape, "op1Waveshape",
		 eParamTypeFix, 0x00000000, 0x00030000,
		 32);

  param_desc_aux(desc, eParam_op2Mod1Source, "op2Mod1Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op2Mod1Gain, "op2Mod1Gain");
  param_desc_aux(desc, eParam_op2Mod2Source, "op2Mod2Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op2Mod2Gain, "op2Mod2Gain");
  param_desc_ratio(eParam_op2Tune, "op2Tune");
  param_desc_adsrTime(eParam_op2Attack, "op2Attack");
  param_desc_adsrTime(eParam_op2Decay, "op2Decay");
  param_desc_fader(eParam_op2Sustain, "op2Sustain");
  param_desc_adsrTime(eParam_op2Release, "op2Release");
  param_desc_aux(desc, eParam_op2Waveshape, "op2Waveshape",
		 eParamTypeFix, 0x00000000, 0x00030000,
		 32);

  param_desc_aux(desc, eParam_op3Mod1Source, "op3Mod1Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op3Mod1Gain, "op3Mod1Gain");
  param_desc_aux(desc, eParam_op3Mod2Source, "op3Mod2Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op3Mod2Gain, "op3Mod2Gain");
  param_desc_ratio(eParam_op3Tune, "op3Tune");
  param_desc_adsrTime(eParam_op3Attack, "op3Attack");
  param_desc_adsrTime(eParam_op3Decay, "op3Decay");
  param_desc_fader(eParam_op3Sustain, "op3Sustain");
  param_desc_adsrTime(eParam_op3Release, "op3Release");
  param_desc_aux(desc, eParam_op3Waveshape, "op3Waveshape",
		 eParamTypeFix, 0x00000000, 0x00030000,
		 32);

  param_desc_aux(desc, eParam_op4Mod1Source, "op4Mod1Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op4Mod1Gain, "op4Mod1Gain");
  param_desc_aux(desc, eParam_op4Mod2Source, "op4Mod2Source",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader(eParam_op4Mod2Gain, "op4Mod2Gain");
  param_desc_ratio(eParam_op4Tune, "op4Tune");
  param_desc_adsrTime(eParam_op4Attack, "op4Attack");
  param_desc_adsrTime(eParam_op4Decay, "op4Decay");
  param_desc_fader(eParam_op4Sustain, "op4Sustain");
  param_desc_adsrTime(eParam_op4Release, "op4Release");
  param_desc_aux(desc, eParam_op4Waveshape, "op4Waveshape",
		 eParamTypeFix, 0x00000000, 0x00030000,
		 32);

  param_desc_aux(desc, eParam_lfoSpeed, "LFOSpeed",
		 eParamTypeFix, 0, FR32_MAX,
		 16);
  param_desc_pan(eParam_lfoWaveshape, "lfoWaveshape");

}

// EOF
