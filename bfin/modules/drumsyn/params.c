#include <string.h>
#include "drumsyn.h"
#include "env_int.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 


// set parameter by value (fix16)
void module_set_param(u32 idx, pval v) {
  u8 b;
  switch(idx) {
  case eParamGate0 :
    b = v.s > 0;
#if 0
    env_int_set_gate(voices[0]->envAmp, b);
    env_int_set_gate(voices[0]->envFreq, b);
    env_int_set_gate(voices[0]->envRes, b);
#else // testing
    if(b) {
      filter_1p_fr32_in(&ampLp, FR32_MAX >> 1);
    } else {
      filter_1p_fr32_in(&ampLp, 0);
    }
#endif
    break;
  case eParamAmp0 :
    voices[0]->amp = v.fr;
    break;
  case eParamAmpAtkDur0 : // in samples
    env_int_set_atk_coeff(voices[0]->envAmp, v.fr);
    break;
  case eParamAmpRelDur0 :
    env_int_set_rel_coeff(voices[0]->envAmp, v.fr);
    break;
  case eParamAmpAtkCurve0 :
    // TODO
    break;
  case eParamAmpRelCurve0 :
    // TODO
    break;
    // freq env
  case eParamFreqAtkDur0 :
    env_int_set_atk_coeff(voices[0]->envFreq, v.fr);
    break;
  case eParamFreqRelDur0 :
    env_int_set_rel_coeff(voices[0]->envFreq, v.fr);
    break;
  case eParamFreqAtkCurve0 :
    // TODO
    break;
  case eParamFreqRelCurve0 :
    // TODO
    break;
  case eParamFreqMul0 :
    //    voices[0]->envMulFreq = v.fr;
    env_int_set_scale(voices[0]->envFreq, v.fr);
    break;
  case eParamFreqAdd0 :
    voices[0]->envAddFreq = v.fr;
    break;
    // res env
  case eParamResAtkDur0 :
    env_int_set_atk_coeff(voices[0]->envRes, v.fr);
    break;
  case eParamResRelDur0 :
    env_int_set_rel_coeff(voices[0]->envRes, v.fr);
    break;
  case eParamResAtkCurve0 :
    // TODO
    break;
  case eParamResRelCurve0 :
    // TODO
    break;
  case eParamResMul0 :
    //    voices[0]->envMulRes = v.fr;
    env_int_set_scale(voices[0]->envRes, v.fr);
    break;
  case eParamResAdd0 :
    voices[0]->envAddRes = v.fr;
    break;
    //// TODO
    /// .. other voices...

  default:
    break;
  }
}



void fill_param_desc(void) {
  strcpy(gModuleData->paramDesc[eParamGate0].label, "Gate0");
  strcpy(gModuleData->paramDesc[eParamGate0].unit, "");
  gModuleData->paramDesc[eParamGate0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamGate0].min = 0;
  gModuleData->paramDesc[eParamGate0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamTrig0].label, "Trig0");
  strcpy(gModuleData->paramDesc[eParamTrig0].unit, "");
  gModuleData->paramDesc[eParamTrig0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamTrig0].min = 0;
  gModuleData->paramDesc[eParamTrig0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamAmp0].label, "Amp0");
  strcpy(gModuleData->paramDesc[eParamAmp0].unit, "");
  gModuleData->paramDesc[eParamAmp0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmp0].min = 0;
  gModuleData->paramDesc[eParamAmp0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamAmpAtkDur0].label, "AmpAtkDur0");
  strcpy(gModuleData->paramDesc[eParamAmpAtkDur0].unit, "");
  gModuleData->paramDesc[eParamAmpAtkDur0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmpAtkDur0].min = 0;
  gModuleData->paramDesc[eParamAmpAtkDur0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamAmpRelDur0].label, "AmpRelDur0");
  strcpy(gModuleData->paramDesc[eParamAmpRelDur0].unit, "");
  gModuleData->paramDesc[eParamAmpRelDur0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmpRelDur0].min = 0;
  gModuleData->paramDesc[eParamAmpRelDur0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamAmpAtkCurve0].label, "AmpAtkCurve0");
  strcpy(gModuleData->paramDesc[eParamAmpAtkCurve0].unit, "");
  gModuleData->paramDesc[eParamAmpAtkCurve0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmpAtkCurve0].min = 0;
  gModuleData->paramDesc[eParamAmpAtkCurve0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamAmpRelCurve0].label, "AmpRelCurve0");
  strcpy(gModuleData->paramDesc[eParamAmpRelCurve0].unit, "");
  gModuleData->paramDesc[eParamAmpRelCurve0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmpRelCurve0].min = 0;
  gModuleData->paramDesc[eParamAmpRelCurve0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqAtkDur0].label, "FreqAtkDur0");
  strcpy(gModuleData->paramDesc[eParamFreqAtkDur0].unit, "");
  gModuleData->paramDesc[eParamFreqAtkDur0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqAtkDur0].min = 0;
  gModuleData->paramDesc[eParamFreqAtkDur0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqRelDur0].label, "FreqRelDur0");
  strcpy(gModuleData->paramDesc[eParamFreqRelDur0].unit, "");
  gModuleData->paramDesc[eParamFreqRelDur0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqRelDur0].min = 0;
  gModuleData->paramDesc[eParamFreqRelDur0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqAtkCurve0].label, "FreqAtkCurve0");
  strcpy(gModuleData->paramDesc[eParamFreqAtkCurve0].unit, "");
  gModuleData->paramDesc[eParamFreqAtkCurve0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqAtkCurve0].min = 0;
  gModuleData->paramDesc[eParamFreqAtkCurve0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqRelCurve0].label, "FreqRelCurve0");
  strcpy(gModuleData->paramDesc[eParamFreqRelCurve0].unit, "");
  gModuleData->paramDesc[eParamFreqRelCurve0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqRelCurve0].min = 0;
  gModuleData->paramDesc[eParamFreqRelCurve0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqMul0].label, "FreqMul0");
  strcpy(gModuleData->paramDesc[eParamFreqMul0].unit, "");
  gModuleData->paramDesc[eParamFreqMul0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqMul0].min = 0;
  gModuleData->paramDesc[eParamFreqMul0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqAdd0].label, "FreqAdd0");
  strcpy(gModuleData->paramDesc[eParamFreqAdd0].unit, "");
  gModuleData->paramDesc[eParamFreqAdd0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqAdd0].min = 0;
  gModuleData->paramDesc[eParamFreqAdd0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResAtkDur0].label, "ResAtkDur0");
  strcpy(gModuleData->paramDesc[eParamResAtkDur0].unit, "");
  gModuleData->paramDesc[eParamResAtkDur0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResAtkDur0].min = 0;
  gModuleData->paramDesc[eParamResAtkDur0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResRelDur0].label, "ResRelDur0");
  strcpy(gModuleData->paramDesc[eParamResRelDur0].unit, "");
  gModuleData->paramDesc[eParamResRelDur0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResRelDur0].min = 0;
  gModuleData->paramDesc[eParamResRelDur0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResAtkCurve0].label, "ResAtkCurve0");
  strcpy(gModuleData->paramDesc[eParamResAtkCurve0].unit, "");
  gModuleData->paramDesc[eParamResAtkCurve0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResAtkCurve0].min = 0;
  gModuleData->paramDesc[eParamResAtkCurve0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResRelCurve0].label, "ResRelCurve0");
  strcpy(gModuleData->paramDesc[eParamResRelCurve0].unit, "");
  gModuleData->paramDesc[eParamResRelCurve0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResRelCurve0].min = 0;
  gModuleData->paramDesc[eParamResRelCurve0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResMul0].label, "ResMul0");
  strcpy(gModuleData->paramDesc[eParamResMul0].unit, "");
  gModuleData->paramDesc[eParamResMul0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResMul0].min = 0;
  gModuleData->paramDesc[eParamResMul0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResAdd0].label, "ResAdd0");
  strcpy(gModuleData->paramDesc[eParamResAdd0].unit, "");
  gModuleData->paramDesc[eParamResAdd0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResAdd0].min = 0;
  gModuleData->paramDesc[eParamResAdd0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamLow0].label, "Low0");
  strcpy(gModuleData->paramDesc[eParamLow0].unit, "");
  gModuleData->paramDesc[eParamLow0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamLow0].min = 0;
  gModuleData->paramDesc[eParamLow0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamHigh0].label, "High0");
  strcpy(gModuleData->paramDesc[eParamHigh0].unit, "");
  gModuleData->paramDesc[eParamHigh0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamHigh0].min = 0;
  gModuleData->paramDesc[eParamHigh0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamBand0].label, "Band0");
  strcpy(gModuleData->paramDesc[eParamBand0].unit, "");
  gModuleData->paramDesc[eParamBand0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamBand0].min = 0;
  gModuleData->paramDesc[eParamBand0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamNotch0].label, "Notch0");
  strcpy(gModuleData->paramDesc[eParamNotch0].unit, "");
  gModuleData->paramDesc[eParamNotch0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamNotch0].min = 0;
  gModuleData->paramDesc[eParamNotch0].max = FR32_MAX;

  /// TODO: more voices
}
