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
#define GRAINS_PATCHPOINTS 15
#define param_desc_patch(param_idx, desc_string) param_desc_aux (desc, param_idx, desc_string, eParamTypeLabel, 0, GRAINS_PATCHPOINTS-1, 1)
#define param_desc_fader(param_idx, desc_string)  param_desc_aux (desc, param_idx, desc_string, FADER_TYPE, FADER_MIN, FADER_MAX, FADER_RADIX)
#define param_desc_pan(param_idx, desc_string)  param_desc_aux (desc, param_idx, desc_string, PAN_TYPE, PAN_MIN, PAN_MAX, PAN_RADIX)

extern void fill_param_desc(ParamDesc* desc) {

  //CV Sources
  param_desc_patch(eParam_cvPatch1, "source_cv1");
  param_desc_patch(eParam_cvPatch2, "source_cv2");
  param_desc_patch(eParam_cvPatch3, "source_cv3");
  param_desc_patch(eParam_cvPatch4, "source_cv4");

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

  param_desc_patch(eParam_source_i3, "source_i3");
  param_desc_fader (eParam_fader_i3, "fader_i3");
  param_desc_pan (eParam_pan_i3, "pan_i3");
  param_desc_fader (eParam_aux1_i3, "aux1_i3");
  param_desc_fader (eParam_aux2_i3, "aux2_i3");
  param_desc_fader (eParam_effect_i3, "effect_i3");

  param_desc_patch(eParam_source_i4, "source_i4");
  param_desc_fader (eParam_fader_i4, "fader_i4");
  param_desc_pan (eParam_pan_i4, "pan_i4");
  param_desc_fader (eParam_aux1_i4, "aux1_i4");
  param_desc_fader (eParam_aux2_i4, "aux2_i4");
  param_desc_fader (eParam_effect_i4, "effect_i4");

  //Grain Mixer Params
  param_desc_patch(eParam_source_g1, "source_g1");
  param_desc_fader (eParam_fader_g1, "fader_g1");
  param_desc_pan (eParam_pan_g1, "pan_g1");
  param_desc_fader (eParam_aux1_g1, "aux1_g1");
  param_desc_fader (eParam_aux2_g1, "aux2_g1");
  param_desc_fader (eParam_effect_g1, "effect_g1");
  param_desc_aux(desc, eParam_phase_g1, "phase_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 16);
  param_desc_aux (desc, eParam_lpf_g1, "lpf_g1",
		  eParamTypeFix, 50 << 16, 8000 << 16, 16);
  
  //grain mod params
  param_desc_patch(eParam_FM_source_g1, "FM_source_g1");
  param_desc_fader(eParam_FM_level_g1, "FM_level_g1");
  param_desc_patch(eParam_AM_source_g1, "AM_source_g1");
  param_desc_pan(eParam_AM_level_g1, "AM_level_g1");

  //Scrub Params
  param_desc_aux(desc, eParam_scrubEnable_g1, "scrubEnable_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux (desc, eParam_scrubPitch_g1, "scrubPitch_g1",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);
  param_desc_aux (desc, eParam_scrubLength_g1, "scrubLength_g1",
		  TIME_SAMPLE_TYPE, TIME_SAMPLE_MIN, TIME_SAMPLE_MAX,
		  TIME_SAMPLE_RADIX);
  param_desc_aux(desc, eParam_scrubPitchDetection_g1, "scrubPitchDetection_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 16);

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
  //FIXME - edge bounce is currently pretty useless so disable for now
  param_desc_aux(desc, eParam_echoEdgeBehaviour_g1, "echoWrap_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
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

  //Tracking Oscillator Params
  param_desc_aux(desc,   eParam_envAttack_g1, "envAttack_g1",
		 eParamTypeFix, 0x00010000, 0x00960000,
		 32);
  param_desc_aux(desc, eParam_trackingEnv_g1, "trackingEnv_g1",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux (desc, eParam_trackingPitch_g1, "trackingPitch_g1",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);
  
  //Grain Mixer Params
  param_desc_patch(eParam_source_g2, "source_g2");
  param_desc_fader (eParam_fader_g2, "fader_g2");
  param_desc_pan (eParam_pan_g2, "pan_g2");
  param_desc_fader (eParam_aux1_g2, "aux1_g2");
  param_desc_fader (eParam_aux2_g2, "aux2_g2");
  param_desc_fader (eParam_effect_g2, "effect_g2");
  param_desc_aux(desc, eParam_phase_g2, "phase_g2",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 16);
  param_desc_aux (desc, eParam_lpf_g2, "lpf_g2",
		  eParamTypeFix, 50 << 16, 8000 << 16, 16);
  
  //grain mod params
  param_desc_patch(eParam_FM_source_g2, "FM_source_g2");
  param_desc_fader(eParam_FM_level_g2, "FM_level_g2");
  param_desc_patch(eParam_AM_source_g2, "AM_source_g2");
  param_desc_pan(eParam_AM_level_g2, "AM_level_g2");;
  
  //Scrub Params
  param_desc_aux(desc, eParam_scrubEnable_g2, "scrubEnable_g2",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux (desc, eParam_scrubPitch_g2, "scrubPitch_g2",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);
  param_desc_aux (desc, eParam_scrubLength_g2, "scrubLength_g2",
		  TIME_SAMPLE_TYPE, TIME_SAMPLE_MIN, TIME_SAMPLE_MAX,
		  TIME_SAMPLE_RADIX);
  param_desc_aux(desc, eParam_scrubPitchDetection_g2, "scrubPitchDetection_g2",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 16);

  //Echo Params
  param_desc_aux (desc, eParam_echoFadeLength_g2, "echoFadeLength_g2",
		  eParamTypeFix, 0x00000000, FR32_MAX,
		  8);

  param_desc_aux (desc, eParam_echoTime_g2, "echoTime_g2",
		  TIME_MS_TYPE, TIME_MS_MIN, TIME_MS_MAX,
		  TIME_MS_RADIX);
  param_desc_aux (desc, eParam_echoSpeed_g2, "echoSpeed_g2",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);
  //FIXME - edge bounce is currently pretty useless so disable for now
  param_desc_aux(desc, eParam_echoEdgeBehaviour_g2, "echoWrap_g2",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux (desc, eParam_echoMin_g2, "echoMin_g2",
		  TIME_MS_TYPE, TIME_MS_MIN, TIME_MS_MAX,
		  TIME_MS_RADIX);
  param_desc_aux (desc, eParam_echoMax_g2, "echoMax_g2",
		  TIME_MS_TYPE, TIME_MS_MIN, TIME_MS_MAX,
		  TIME_MS_RADIX);
  param_desc_aux(desc, eParam_writeEnable_g2, "writeEnable_g2",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);

  //Tracking Oscillator Params
  param_desc_aux(desc,   eParam_envAttack_g2, "envAttack_g2",
		 eParamTypeFix, 0x00010000, 0x00960000,
		 32);
  param_desc_aux(desc, eParam_trackingEnv_g2, "trackingEnv_g2",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux (desc, eParam_trackingPitch_g2, "trackingPitch_g2",
		  SPEED_TYPE, SPEED_MIN, SPEED_MAX,
		  SPEED_RADIX);

  param_desc_aux(desc, eParam_LFO_speed, "LFO_speed",
		 eParamTypeFix, 0, FR32_MAX,
		 16);
  param_desc_pan(eParam_LFO_shape, "LFO_shape");

  param_desc_aux(desc, eParam_noiseBurst, "noiseBurst",
		 eParamTypeFix, 0x00000000, 0x00010000,
		 32);
  param_desc_aux(desc, eParam_noiseBurstDecay, "noiseBurstDecay",
		 eParamTypeFix, 0, 0x03E80000,
		 16);
  param_desc_aux (desc, eParam_CV_gen1, "static_CV1",
		  eParamTypeFix, -1 << 31, FR32_MAX,
		  16);
  param_desc_aux (desc, eParam_CV_gen2, "static_CV2",
		  eParamTypeFix, -1 << 31, FR32_MAX,
		  16);

}

// EOF
