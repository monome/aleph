#include <string.h>
#include "module.h"
#include "params.h" 

// set parameter by value (fix16)
void module_set_param(u32 idx, pval v) {

#if 0

  switch(idx) {
  case eParamGate:
    env_asr_set_gate(&(voice[0].ampEnv), v.s > 0);
    break;
  case eParamSvfHz :
    filter_svf_set_hz(&(voice[0].svf), v.fix);
    break;
  case eParamSvfRq :
    filter_svf_set_rq(&(voice[0].svf), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfLow :
    filter_svf_set_low(&(voice[0].svf), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfHigh :
    filter_svf_set_high(&(voice[0].svf), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfBand :
    filter_svf_set_band(&(voice[0].svf), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfNotch :
    filter_svf_set_notch(&(voice[0].svf), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfPeak :
    filter_svf_set_peak(&(voice[0].svf), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamNoiseAmp :
    noiseAmp = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp0 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp1 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp2 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp3 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamAtkDur:
    env_asr_set_atk_dur(ampEnv, sec_to_frames_trunc(v.fix));
    break;
  case eParamRelDur:
    env_asr_set_rel_dur(ampEnv, sec_to_frames_trunc(v.fix));
    break;
  case eParamAtkCurve:
    env_asr_set_atk_shape(ampEnv, FIX16_FRACT_TRUNC(BIT_ABS(v.fix)));
    break;
  case eParamRelCurve:
    env_asr_set_atk_shape(ampEnv, FIX16_FRACT_TRUNC(BIT_ABS(v.fix)));
    break;

  default:
    break;
  }

#endif

}


void fill_param_desc(void) {


#if 0

  strcpy(gModuleData->paramDesc[eParamGate].label, "gate");
  strcpy(gModuleData->paramDesc[eParamGate].unit, "");
  gModuleData->paramDesc[eParamGate].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamGate].min = 0;
  gModuleData->paramDesc[eParamGate].max = FIX16_ONE;
  
  strcpy(gModuleData->paramDesc[eParamAtkDur].label, "amp env atk");
  strcpy(gModuleData->paramDesc[eParamAtkDur].unit, "sec");
  gModuleData->paramDesc[eParamAtkDur].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAtkDur].min = 0;
  gModuleData->paramDesc[eParamAtkDur].max = FIX16_ONE;
  
  strcpy(gModuleData->paramDesc[eParamRelDur].label, "amp env rel");
  strcpy(gModuleData->paramDesc[eParamRelDur].unit, "hz");
  gModuleData->paramDesc[eParamRelDur].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRelDur].min = 0;
  gModuleData->paramDesc[eParamRelDur].max = FIX16_ONE;
  
  strcpy(gModuleData->paramDesc[eParamAtkCurve].label, "amp env atk curve");
  strcpy(gModuleData->paramDesc[eParamAtkCurve].unit, "hz");
  gModuleData->paramDesc[eParamAtkCurve].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAtkCurve].min = 0;
  gModuleData->paramDesc[eParamAtkCurve].max = FIX16_ONE;
  
  strcpy(gModuleData->paramDesc[eParamRelCurve].label, "amp env rel curve");
  strcpy(gModuleData->paramDesc[eParamRelCurve].unit, "hz");
  gModuleData->paramDesc[eParamRelCurve].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRelCurve].min = 0;
  gModuleData->paramDesc[eParamRelCurve].max = FIX16_ONE;
  

  /* strcpy(gModuleData->paramDesc[eParamSvfHz].label, "cutoff"); */
  /* strcpy(gModuleData->paramDesc[eParamSvfHz].unit, "hz"); */
  /* gModuleData->paramDesc[eParamSvfHz].type = PARAM_TYPE_FIX; */
  /* gModuleData->paramDesc[eParamSvfHz].min = SVF_HZ_MIN; */
  /* gModuleData->paramDesc[eParamSvfHz].max = SVF_HZ_MAX; */
  /// FIXME: svf corner coeff is dumb right now
  strcpy(gModuleData->paramDesc[eParamSvfHz].label, "cutoff");
  strcpy(gModuleData->paramDesc[eParamSvfHz].unit, "");
  gModuleData->paramDesc[eParamSvfHz].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfHz].min = 0;
  gModuleData->paramDesc[eParamSvfHz].max = 0x8000;

  strcpy(gModuleData->paramDesc[eParamSvfRq].label, "rq");
  strcpy(gModuleData->paramDesc[eParamSvfRq].unit, "");
  gModuleData->paramDesc[eParamSvfRq].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfRq].min = 0;
  gModuleData->paramDesc[eParamSvfRq].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfLow].label, "low");
  strcpy(  gModuleData->paramDesc[eParamSvfLow].unit, "");
  gModuleData->paramDesc[eParamSvfLow].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfLow].min = 0;
  gModuleData->paramDesc[eParamSvfLow].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfHigh].label, "high");
  strcpy(  gModuleData->paramDesc[eParamSvfHigh].unit, "");
  gModuleData->paramDesc[eParamSvfHigh].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfHigh].min = 0;
  gModuleData->paramDesc[eParamSvfHigh].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfBand].label, "band");
  strcpy(  gModuleData->paramDesc[eParamSvfBand].unit, "");
  gModuleData->paramDesc[eParamSvfBand].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfBand].min = 0;
  gModuleData->paramDesc[eParamSvfBand].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfNotch].label, "notch");
  strcpy(  gModuleData->paramDesc[eParamSvfNotch].unit, "");
  gModuleData->paramDesc[eParamSvfNotch].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfNotch].min = 0;
  gModuleData->paramDesc[eParamSvfNotch].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfPeak].label, "peak");
  strcpy(  gModuleData->paramDesc[eParamSvfPeak].unit, "");
  gModuleData->paramDesc[eParamSvfPeak].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfPeak].min = 0;
  gModuleData->paramDesc[eParamSvfPeak].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamNoiseAmp].label, "noise");
  strcpy(  gModuleData->paramDesc[eParamNoiseAmp].unit, "");
  gModuleData->paramDesc[eParamNoiseAmp].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamNoiseAmp].min = 0;
  gModuleData->paramDesc[eParamNoiseAmp].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamInAmp0].label, "in0");
  strcpy(  gModuleData->paramDesc[eParamInAmp0].unit, "");
  gModuleData->paramDesc[eParamInAmp0].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp0].min = 0;
  gModuleData->paramDesc[eParamInAmp0].max = fix16_one;

  strcpy( gModuleData->paramDesc[eParamInAmp1].label, "in1");
  strcpy(  gModuleData->paramDesc[eParamInAmp1].unit, "");
  gModuleData->paramDesc[eParamInAmp1].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp1].min = 0;
  gModuleData->paramDesc[eParamInAmp1].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamInAmp2].label, "in2");
  strcpy(  gModuleData->paramDesc[eParamInAmp2].unit, "");
  gModuleData->paramDesc[eParamInAmp2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp2].min = 0;
  gModuleData->paramDesc[eParamInAmp2].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamInAmp3].label, "in3");
  strcpy(  gModuleData->paramDesc[eParamInAmp3].unit, "");
  gModuleData->paramDesc[eParamInAmp3].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp3].min = 0;
  gModuleData->paramDesc[eParamInAmp3].max = fix16_one;

#endif

}

// EOF
