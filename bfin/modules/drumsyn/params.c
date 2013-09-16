#include <string.h>
#include "drumsyn.h"
#include "env_int.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 


#ifdef DRUMSYN_NOENV

// set parameter by value
void module_set_param(u32 idx, pval v) {
  drumsynVoice* vp; // 	tmp voice pointer
  u8 b; // 		tmp bit

  switch(idx) {

  case eParamGate0 : // 1bit gate
    vp = voices[0];
    b = v.s > 0;
    if(b) { 
      filter_1p_fr32_set_slew( &(vp->lpAmp) , vp->ampSlewUp);
      filter_1p_fr32_in( &(vp->lpAmp), vp->amp);
      filter_1p_fr32_in( &(vp->lpAmp), vp->amp);
    } else {
      filter_1p_fr32_set_slew( &(vp->lpAmp) , vp->ampSlewDown);
      filter_1p_fr32_in( &(vp->lpAmp), 0);
      filter_1p_fr32_in( &(vp->lpAmp), 0);
    }
    break;
    
  case eParamAmp0 : // fract32 amp
    vp = voices[0];
    vp->amp = v.fr;
    if(vp->lpAmp.y > v.fr) { vp->lpAmp.y = v.fr; }
    break;
    
  case eParamAmpAtkSlew0 : // fract32 raw 1pole coefficient
    voices[0]->ampSlewUp = v.s;
    break;
    
  case eParamAmpRelSlew0 :
    voices[0]->ampSlewDown = v.s;
    break;

    // freq env
  case eParamFreqAtkSlew0 :
    voices[0]->freqSlewUp = v.s;
    break;

  case eParamFreqRelSlew0 :
    voices[0]->freqSlewDown = v.s;
    break;

  case eParamFreqStart0 :
    // release target
    vp->freqOff = v.fr;

    break;
  case eParamFreqEnd0 :
    vp->freqOn = v.fr;
    break;

    // res env
  case eParamResAtkSlew0 :

    break;
  case eParamResRelSlew0 :

    break;

  case eParamResStart0 :
    vp->resOff = v.fr;
    //    env_int_set_scale(vp->envRes, v.fr);
    break;
  case eParamResEnd0 :
    vp->resOn = v.fr;
    break;
    //// TODO
    /// .. other voices...
  default:
    break;
  }


}

#else
// set parameter by value (fix16)
void module_set_param(u32 idx, pval v) {
  drumsynVoice* vp; // 	tmp voice pointer
  u8 b; // 		tmp bit

  switch(idx) {
  case eParamGate0 :
    b = v.s > 0;
    vp = &(voices[0]);
/* #ifdef DRUMSYN_NOENV */
/*     if(b) {  */
/*       filter_1p_fr32_set_coeff( &(vp->lpAmp) , voices[0]-> */
/*       filter_1p_fr32_in( &(vp->lpAmp), vp->amp); */
/*       filter_1p_fr32_in( &(vp->lpAmp), vp->amp); */
/*     } else { */
/*       filter_1p_fr32_in( &(voices[0]->lpAmp), 0); */
/*     } */
/* #else */
    env_int_set_gate(voices[0]->envAmp, b);
    env_int_set_gate(voices[0]->envFreq, b);
    env_int_set_gate(voices[0]->envRes, b);
    //#endif
    break;
  case eParamAmp0 :
    voices[0]->amp = v.fr;
    break;
  case eParamAmpAtkSlew0 : // raw 1pole coefficient
    //#ifdef DRUMSYN_NOENV
    /* if(b) { */
    /*   filter_1p_fr32_in( &(voices[0]->lpAmp), voices[0]->amp); */
    /*   filter_1p_fr32_in( &(voices[0]->lpAmp), voices[0]->amp); */
    /* } else { */
    /*   filter_1p_fr32_in( &(voices[0]->lpAmp), 0); */
    /* } */
    ///#else

    env_int_set_atk_slew(voices[0]->envAmp, v.fr);
    break;
  case eParamAmpRelSlew0 :
    
    //    env_int_set_rel_slew(voices[0]->envAmp, v.fr);
    break;
  case eParamAmpAtkCurve0 :
    // TODO
    break;
  case eParamAmpRelCurve0 :
    // TODO
    break;
    // freq env
  case eParamFreqAtkSlew0 :
    env_int_set_atk_slew(voices[0]->envFreq, v.fr);
    break;
  case eParamFreqRelSlew0 :
    env_int_set_rel_slew(voices[0]->envFreq, v.fr);
    break;
  case eParamFreqAtkCurve0 :
    // TODO
    break;
  case eParamFreqRelCurve0 :
    // TODO
    break;
  case eParamFreqStart0 :
    //    voices[0]->envStartFreq = v.fr;
    env_int_set_scale(voices[0]->envFreq, v.fr);
    break;
  case eParamFreqEnd0 :
    voices[0]->envEndFreq = v.fr;
    break;
    // res env
  case eParamResAtkSlew0 :
    env_int_set_atk_slew(voices[0]->envRes, v.fr);
    break;
  case eParamResRelSlew0 :
    env_int_set_rel_slew (voices[0]->envRes, v.fr);
    break;
  case eParamResAtkCurve0 :
    // TODO
    break;
  case eParamResRelCurve0 :
    // TODO
    break;
  case eParamResStart0 :
    //    voices[0]->envStartRes = v.fr;
    env_int_set_scale(voices[0]->envRes, v.fr);
    break;
  case eParamResEnd0 :
    voices[0]->envEndRes = v.fr;
    break;
    //// TODO
    /// .. other voices...

  default:
    break;
  }
}




#endif // DRUMSYN_NOENV

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

  strcpy(gModuleData->paramDesc[eParamAmpAtkSlew0].label, "AmpAtkSlew0");
  strcpy(gModuleData->paramDesc[eParamAmpAtkSlew0].unit, "");
  gModuleData->paramDesc[eParamAmpAtkSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmpAtkSlew0].min = 0;
  gModuleData->paramDesc[eParamAmpAtkSlew0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamAmpRelSlew0].label, "AmpRelSlew0");
  strcpy(gModuleData->paramDesc[eParamAmpRelSlew0].unit, "");
  gModuleData->paramDesc[eParamAmpRelSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamAmpRelSlew0].min = 0;
  gModuleData->paramDesc[eParamAmpRelSlew0].max = FR32_MAX;

  /* strcpy(gModuleData->paramDesc[eParamAmpAtkCurve0].label, "AmpAtkCurve0"); */
  /* strcpy(gModuleData->paramDesc[eParamAmpAtkCurve0].unit, ""); */
  /* gModuleData->paramDesc[eParamAmpAtkCurve0].type = PARAM_TYPE_FRACT; */
  /* gModuleData->paramDesc[eParamAmpAtkCurve0].min = 0; */
  /* gModuleData->paramDesc[eParamAmpAtkCurve0].max = FR32_MAX; */

  /* strcpy(gModuleData->paramDesc[eParamAmpRelCurve0].label, "AmpRelCurve0"); */
  /* strcpy(gModuleData->paramDesc[eParamAmpRelCurve0].unit, ""); */
  /* gModuleData->paramDesc[eParamAmpRelCurve0].type = PARAM_TYPE_FRACT; */
  /* gModuleData->paramDesc[eParamAmpRelCurve0].min = 0; */
  /* gModuleData->paramDesc[eParamAmpRelCurve0].max = FR32_MAX; */

  strcpy(gModuleData->paramDesc[eParamFreqAtkSlew0].label, "FreqAtkSlew0");
  strcpy(gModuleData->paramDesc[eParamFreqAtkSlew0].unit, "");
  gModuleData->paramDesc[eParamFreqAtkSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqAtkSlew0].min = 0;
  gModuleData->paramDesc[eParamFreqAtkSlew0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqRelSlew0].label, "FreqRelSlew0");
  strcpy(gModuleData->paramDesc[eParamFreqRelSlew0].unit, "");
  gModuleData->paramDesc[eParamFreqRelSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqRelSlew0].min = 0;
  gModuleData->paramDesc[eParamFreqRelSlew0].max = FR32_MAX;

  /* strcpy(gModuleData->paramDesc[eParamFreqAtkCurve0].label, "FreqAtkCurve0"); */
  /* strcpy(gModuleData->paramDesc[eParamFreqAtkCurve0].unit, ""); */
  /* gModuleData->paramDesc[eParamFreqAtkCurve0].type = PARAM_TYPE_FRACT; */
  /* gModuleData->paramDesc[eParamFreqAtkCurve0].min = 0; */
  /* gModuleData->paramDesc[eParamFreqAtkCurve0].max = FR32_MAX; */

  /* strcpy(gModuleData->paramDesc[eParamFreqRelCurve0].label, "FreqRelCurve0"); */
  /* strcpy(gModuleData->paramDesc[eParamFreqRelCurve0].unit, ""); */
  /* gModuleData->paramDesc[eParamFreqRelCurve0].type = PARAM_TYPE_FRACT; */
  /* gModuleData->paramDesc[eParamFreqRelCurve0].min = 0; */
  /* gModuleData->paramDesc[eParamFreqRelCurve0].max = FR32_MAX; */

  strcpy(gModuleData->paramDesc[eParamFreqStart0].label, "FreqStart0");
  strcpy(gModuleData->paramDesc[eParamFreqStart0].unit, "");
  gModuleData->paramDesc[eParamFreqStart0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqStart0].min = 0;
  gModuleData->paramDesc[eParamFreqStart0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqEnd0].label, "FreqEnd0");
  strcpy(gModuleData->paramDesc[eParamFreqEnd0].unit, "");
  gModuleData->paramDesc[eParamFreqEnd0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqEnd0].min = 0;
  gModuleData->paramDesc[eParamFreqEnd0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResAtkSlew0].label, "ResAtkSlew0");
  strcpy(gModuleData->paramDesc[eParamResAtkSlew0].unit, "");
  gModuleData->paramDesc[eParamResAtkSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResAtkSlew0].min = 0;
  gModuleData->paramDesc[eParamResAtkSlew0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResRelSlew0].label, "ResRelSlew0");
  strcpy(gModuleData->paramDesc[eParamResRelSlew0].unit, "");
  gModuleData->paramDesc[eParamResRelSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResRelSlew0].min = 0;
  gModuleData->paramDesc[eParamResRelSlew0].max = FR32_MAX;

  /* strcpy(gModuleData->paramDesc[eParamResAtkCurve0].label, "ResAtkCurve0"); */
  /* strcpy(gModuleData->paramDesc[eParamResAtkCurve0].unit, ""); */
  /* gModuleData->paramDesc[eParamResAtkCurve0].type = PARAM_TYPE_FRACT; */
  /* gModuleData->paramDesc[eParamResAtkCurve0].min = 0; */
  /* gModuleData->paramDesc[eParamResAtkCurve0].max = FR32_MAX; */

  /* strcpy(gModuleData->paramDesc[eParamResRelCurve0].label, "ResRelCurve0"); */
  /* strcpy(gModuleData->paramDesc[eParamResRelCurve0].unit, ""); */
  /* gModuleData->paramDesc[eParamResRelCurve0].type = PARAM_TYPE_FRACT; */
  /* gModuleData->paramDesc[eParamResRelCurve0].min = 0; */
  /* gModuleData->paramDesc[eParamResRelCurve0].max = FR32_MAX; */

  strcpy(gModuleData->paramDesc[eParamResStart0].label, "ResStart0");
  strcpy(gModuleData->paramDesc[eParamResStart0].unit, "");
  gModuleData->paramDesc[eParamResStart0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResStart0].min = 0;
  gModuleData->paramDesc[eParamResStart0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamResEnd0].label, "ResEnd0");
  strcpy(gModuleData->paramDesc[eParamResEnd0].unit, "");
  gModuleData->paramDesc[eParamResEnd0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamResEnd0].min = 0;
  gModuleData->paramDesc[eParamResEnd0].max = FR32_MAX;

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
