#include <string.h>
#include "drumsyn.h"
//#include "env_int.h"
#include "env_exp.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 

static void set_param_gate(drumsynVoice* vp, s32 val) {
  if(val > 0) { 
    lcprng_reset(&(vp->rngH));
    lcprng_reset(&(vp->rngL));

    env_exp_set_gate( &(vp->envAmp)	, 0xff );
    env_exp_set_gate( &(vp->envFreq)	, 0xff );
    env_exp_set_gate( &(vp->envRq)	, 0xff );

    /* filter_1p_fr32_set_slew( &(vp->lpAmp) , vp->ampSlewUp); */
    /* filter_1p_fr32_in( &(vp->lpAmp), vp->amp); */

    /* filter_1p_fr32_set_slew( &(vp->lpFreq) , vp->freqSlewUp); */
    /* filter_1p_fr32_in( &(vp->lpFreq), vp->freqOn); */

    /* filter_1p_fr32_set_slew( &(vp->lpRq) , vp->rqSlewUp); */
    /* filter_1p_fr32_in( &(vp->lpRq), vp->rqOn); */

  } else {

    env_exp_set_gate( &(vp->envAmp)	, 0 );
    env_exp_set_gate( &(vp->envFreq)	, 0 );
    env_exp_set_gate( &(vp->envRq)	, 0 );

    /* filter_1p_fr32_set_slew( &(vp->lpAmp) , vp->ampSlewDown); */
    /* filter_1p_fr32_in( &(vp->lpAmp), 0); */

    /* filter_1p_fr32_set_slew( &(vp->lpFreq) , vp->freqSlewDown); */
    /* filter_1p_fr32_in( &(vp->lpFreq), vp->freqOff); */

    /* filter_1p_fr32_set_slew( &(vp->lpRq) , vp->rqSlewDown); */
    /* filter_1p_fr32_in( &(vp->lpRq), vp->rqOff); */
  }

}

// set parameter by value
void module_set_param(u32 idx, pval v) {
  //  drumsynVoice* vp; // 	tmp voice pointer

  switch(idx) {

  case eParamGate0 : // 1bit gate
    //    vp = voices[0];
    set_param_gate(voices[0], v.s);
    break;
    
  case eParamAmp0 : // fract32 amp
    env_exp_set_on( &(voices[0]->envAmp), v.fr);    
    break;
    
  case eParamAmpSus0 : // fract32 amp
    env_exp_set_sus( &(voices[0]->envAmp), v.fr);    
    break;
    
  case eParamAmpAtkSlew0 : // fract32 raw 1pole coefficient
    env_exp_set_atk_slew( &(voices[0]->envAmp), v.fr);
    break;
    
  case eParamAmpDecSlew0 : // fract32 raw 1pole coefficient
    env_exp_set_dec_slew( &(voices[0]->envAmp), v.fr);
    break;

  case eParamAmpRelSlew0 :
    env_exp_set_rel_slew( &(voices[0]->envAmp), v.fr);
    break;

  case eParamAmpSusDur0 :
    env_exp_set_sus_dur( &(voices[0]->envAmp), v.u);
    break;

    // freq env
  case eParamFreqAtkSlew0 :
    env_exp_set_atk_slew( &(voices[0]->envFreq), v.fr);
    break;

  case eParamFreqDecSlew0 :
    env_exp_set_dec_slew( &(voices[0]->envFreq), v.fr);
    break;

  case eParamFreqRelSlew0 :
    env_exp_set_rel_slew( &(voices[0]->envFreq), v.fr);
    break;

  case eParamFreqSusDur0 :
    env_exp_set_sus_dur( &(voices[0]->envFreq), v.u);
    break;

  case eParamFreqOff0 :
    env_exp_set_off( &(voices[0]->envFreq), v.fr);
    break;
  case eParamFreqOn0: 
    env_exp_set_on( &(voices[0]->envFreq), v.fr);
    break;
  case eParamFreqSus0: 
    env_exp_set_sus( &(voices[0]->envFreq), v.fr);
    break;

    // rq env
  case eParamRqAtkSlew0 :
    env_exp_set_atk_slew( &(voices[0]->envRq), v.fr);
    break;
  case eParamRqDecSlew0 :
    env_exp_set_dec_slew( &(voices[0]->envRq), v.fr);
    break;
  case eParamRqRelSlew0 :
    env_exp_set_rel_slew( &(voices[0]->envRq), v.fr);
    break;
  case eParamRqSusDur0 :
    env_exp_set_sus_dur( &(voices[0]->envRq), v.u);
    break;

  case eParamRqOff0 :
    env_exp_set_off( &(voices[0]->envRq), v.fr);
    break;
  case eParamRqOn0 :
    env_exp_set_on( &(voices[0]->envRq), v.fr);
    break;
  case eParamRqSus0 :
    env_exp_set_sus( &(voices[0]->envRq), v.fr);
    break;

  case eParamLow0 :	       
    filter_svf_set_low( &(voices[0]->svf), v.fr);
    break;
  case eParamHigh0 :	       
    filter_svf_set_high( &(voices[0]->svf), v.fr);
    break;
  case eParamBand0 :	       
    filter_svf_set_band( &(voices[0]->svf), v.fr);
    break;
  case eParamNotch0 :	       
    filter_svf_set_notch( &(voices[0]->svf), v.fr);
    break;


    //// TODO
    /// .. other voices...
  default:
    break;
  }


}

/// FIXME: missing some param descs
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

  strcpy(gModuleData->paramDesc[eParamFreqOff0].label, "FreqOff0");
  strcpy(gModuleData->paramDesc[eParamFreqOff0].unit, "");
  gModuleData->paramDesc[eParamFreqOff0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqOff0].min = 0;
  gModuleData->paramDesc[eParamFreqOff0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamFreqOn0].label, "FreqOn0");
  strcpy(gModuleData->paramDesc[eParamFreqOn0].unit, "");
  gModuleData->paramDesc[eParamFreqOn0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamFreqOn0].min = 0;
  gModuleData->paramDesc[eParamFreqOn0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamRqAtkSlew0].label, "RqAtkSlew0");
  strcpy(gModuleData->paramDesc[eParamRqAtkSlew0].unit, "");
  gModuleData->paramDesc[eParamRqAtkSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamRqAtkSlew0].min = 0;
  gModuleData->paramDesc[eParamRqAtkSlew0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamRqRelSlew0].label, "RqRelSlew0");
  strcpy(gModuleData->paramDesc[eParamRqRelSlew0].unit, "");
  gModuleData->paramDesc[eParamRqRelSlew0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamRqRelSlew0].min = 0;
  gModuleData->paramDesc[eParamRqRelSlew0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamRqOff0].label, "RqOff0");
  strcpy(gModuleData->paramDesc[eParamRqOff0].unit, "");
  gModuleData->paramDesc[eParamRqOff0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamRqOff0].min = 0;
  gModuleData->paramDesc[eParamRqOff0].max = FR32_MAX;

  strcpy(gModuleData->paramDesc[eParamRqOn0].label, "RqOn0");
  strcpy(gModuleData->paramDesc[eParamRqOn0].unit, "");
  gModuleData->paramDesc[eParamRqOn0].type = PARAM_TYPE_FRACT;
  gModuleData->paramDesc[eParamRqOn0].min = 0;
  gModuleData->paramDesc[eParamRqOn0].max = FR32_MAX;

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
