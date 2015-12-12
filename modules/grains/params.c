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

extern void fill_param_desc(ParamDesc* desc) {
  param_desc_fader (eParam_fader_i1, "fader_i1");
  param_desc_pan (eParam_pan_i1, "pan_i1");
  param_desc_fader (eParam_aux1_i1, "aux1_i1");
  param_desc_fader (eParam_aux2_i1, "aux2_i1");
  param_desc_fader (eParam_effect_i1, "effect_i1");

  param_desc_fader (eParam_fader_i2, "fader_i2");
  param_desc_pan (eParam_pan_i2, "pan_i2");
  param_desc_fader (eParam_aux1_i2, "aux1_i2");
  param_desc_fader (eParam_aux2_i2, "aux2_i2");
  param_desc_fader (eParam_effect_i2, "effect_i2");

  param_desc_fader (eParam_fader_i3, "fader_i3");
  param_desc_pan (eParam_pan_i3, "pan_i3");
  param_desc_fader (eParam_aux1_i3, "aux1_i3");
  param_desc_fader (eParam_aux2_i3, "aux2_i3");
  param_desc_fader (eParam_effect_i3, "effect_i3");

  param_desc_fader (eParam_fader_i4, "fader_i4");
  param_desc_pan (eParam_pan_i4, "pan_i4");
  param_desc_fader (eParam_aux1_i4, "aux1_i4");
  param_desc_fader (eParam_aux2_i4, "aux2_i4");
  param_desc_fader (eParam_effect_i4, "effect_i4");

  //Grain Mixer Params
  param_desc_aux(desc, eParam_source_g1, "source_g1",
		 eParamTypeFix, 0x00000000, 0x00040000,
		 32);
  param_desc_fader (eParam_fader_g1, "fader_g1");
  param_desc_pan (eParam_pan_g1, "pan_g1");
  param_desc_fader (eParam_aux1_g1, "aux1_g1");
  param_desc_fader (eParam_aux2_g1, "aux2_g1");
  param_desc_fader (eParam_effect_g1, "effect_g1");
  param_desc_aux(desc, eParam_phase_g1, "phase_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 16);

  //Scrub Params
  param_desc_aux (desc, eParam_scrubPitch_g1, "scrubPitch_g1",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);
  param_desc_aux (desc, eParam_scrubLength_g1, "scrubLength_g1",
		  TIME_SAMPLE_TYPE, TIME_SAMPLE_MIN, TIME_SAMPLE_MAX,
		  TIME_SAMPLE_RADIX);
  param_desc_aux (desc, eParam_scrubFadeLength_g1, "scrubFadeLength_g1",
		  eParamTypeFix, 0x00000000, FR32_MAX,
		  8);

  //Echo Params
  param_desc_aux (desc, eParam_echoFadeLength_g1, "echoFadeLength_g1",
		  eParamTypeFix, 0x00000000, FR32_MAX,
		  8);

  param_desc_aux (desc, eParam_echoTime_g1, "echoTime_g1",
		  TIME_MS_TYPE, TIME_MS_MIN, TIME_MS_MAX,
		  TIME_MS_RADIX);
  param_desc_aux (desc, eParam_echoSpeed_g1, "echoSpeed_g1",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);
  param_desc_aux(desc, eParam_echoEdgeBehaviour_g1, "echoEdgeBehaviour_g1",
		 eParamTypeFix, 0x00000000, 0x00020000,
		 32);
  param_desc_aux (desc, eParam_echoMin_g1, "echoMin_g1",
		  TIME_MS_TYPE, TIME_MS_MIN, TIME_MS_MAX,
		  TIME_MS_RADIX);
  param_desc_aux (desc, eParam_echoMax_g1, "echoMax_g1",
		  TIME_MS_TYPE, TIME_MS_MIN, TIME_MS_MAX,
		  TIME_MS_RADIX);
  param_desc_aux(desc, eParam_writeEnable_g1, "writeEnable_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux(desc, eParam_slewSpeed_g1, "slewSpeed_g1",
		 eParamTypeIntegrator, 0x00000000, FR32_MAX,
		 32);

}

// EOF
