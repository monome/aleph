#include <string.h>
#include "dsyn.h"
//#include "env_int.h"
#include "env_exp.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 



static void set_param_gate(drumsynVoice* vp, s32 val) {
  if(val > 0) { 
    lcprng_reset(&(vp->rngH), 0xDEADFACE);
    //    lcprng_reset(&(vp->rngL), 0xDADABEEF);
    env_exp_set_gate( &(vp->envAmp)	, 0xff );
    env_exp_set_gate( &(vp->envFreq)	, 0xff );
    env_exp_set_gate( &(vp->envRq)	, 0xff );
  } else {
    env_exp_set_gate( &(vp->envAmp)	, 0 );
    env_exp_set_gate( &(vp->envFreq)	, 0 );
    env_exp_set_gate( &(vp->envRq)	, 0 );
  }

}

static void set_param_trig(drumsynVoice* vp, s32 val) {
  u8 b = (val > 0);
  env_exp_set_trig( &(vp->envAmp)	, b );
  env_exp_set_trig( &(vp->envFreq)	, b );
  env_exp_set_trig( &(vp->envRq)	, b );
}

static void set_param_svfpre(drumsynVoice* vp, s32 val) {
  // fixme: flip a function pointer or something?
  if(val > 0) {
    vp->svfPre = 1;
  } else {
    vp->svfPre = 0;
  }
}

static void set_param_freqenv(drumsynVoice* vp, s32 val) {
  // fixme: flip a function pointer or something?
  if(val > 0) {
    vp->freqEnv = 1;
  } else {
    vp->freqEnv = 0;
    filter_svf_set_coeff(&(vp->svf), vp->envFreq.valOn);
  }
}

static void set_param_rqenv(drumsynVoice* vp, s32 val) {
  // fixme: flip a function pointer or something?
  if(val > 0) {
    vp->rqEnv = 1;
  } else {
    vp->rqEnv = 0;
    filter_svf_set_rq(&(vp->svf), vp->envRq.valOn);
  }
}

static void module_set_voice_param(u8 vid, u32 idx, ParamValue v);

// set parameter by value
void module_set_param(u32 idx, ParamValue v) {
  /// offset hack on parameter index
  if(idx < PARAM_VOICE_NPARAMS) {
    module_set_voice_param(0, idx, v);
  } else if (idx < PARAM_VOICE_NPARAMS_x2) {
    module_set_voice_param(1, idx - PARAM_VOICE_NPARAMS, v);
  } else if (idx < PARAM_VOICE_NPARAMS_x3) {
    module_set_voice_param(2, idx - PARAM_VOICE_NPARAMS_x2, v);
  } else {
    module_set_voice_param(3, idx - PARAM_VOICE_NPARAMS_x3, v);
  } 
}

//
static void module_set_voice_param(u8 vid, u32 idx, ParamValue v) {
  switch(idx) {

  case eParamGate0 : // 1bit gate
    set_param_gate(voices[vid], v);
    break;

  case eParamTrig0 : // 1bit trig
    set_param_trig(voices[vid], v);
    break;
    
  case eParamAmp0 : // fract32 amp
    env_exp_set_on( &(voices[vid]->envAmp), v);    
    break;
    
  case eParamAmpSus0 : // fract32 amp
    env_exp_set_sus( &(voices[vid]->envAmp), v);    
    break;
    
  case eParamAmpAtkSlew0 : // fract32 raw 1pole coefficient
    env_exp_set_atk_slew( &(voices[vid]->envAmp), v);
    break;
    
  case eParamAmpDecSlew0 : // fract32 raw 1pole coefficient
    env_exp_set_dec_slew( &(voices[vid]->envAmp), v);
    break;

  case eParamAmpRelSlew0 :
    env_exp_set_rel_slew( &(voices[vid]->envAmp), v);
    break;

  case eParamAmpSusDur0 :
    env_exp_set_sus_dur( &(voices[vid]->envAmp), v);
    break;

    // freq env
  case eParamFreqAtkSlew0 :
    env_exp_set_atk_slew( &(voices[vid]->envFreq), v);
    break;

  case eParamFreqDecSlew0 :
    env_exp_set_dec_slew( &(voices[vid]->envFreq), v);
    break;

  case eParamFreqRelSlew0 :
    env_exp_set_rel_slew( &(voices[vid]->envFreq), v);
    break;

  case eParamFreqSusDur0 :
    env_exp_set_sus_dur( &(voices[vid]->envFreq), v);
    break;

  case eParamFreqOff0 : // fract32 raw SVF corner coefficient
    env_exp_set_off( &(voices[vid]->envFreq), v);
    break;
  case eParamFreqOn0: 
    env_exp_set_on( &(voices[vid]->envFreq), v);
    if(voices[vid]->freqEnv == 0) {
      filter_svf_set_coeff(&(voices[vid]->svf), v);
    }
    break;
  case eParamFreqSus0: 
    env_exp_set_sus( &(voices[vid]->envFreq), v);
    break;

    // rq env
  case eParamRqAtkSlew0 :
    env_exp_set_atk_slew( &(voices[vid]->envRq), v);
    break;
  case eParamRqDecSlew0 :
    env_exp_set_dec_slew( &(voices[vid]->envRq), v);
    break;
  case eParamRqRelSlew0 :
    env_exp_set_rel_slew( &(voices[vid]->envRq), v);
    break;
  case eParamRqSusDur0 :
    env_exp_set_sus_dur( &(voices[vid]->envRq), v);
    break;

  case eParamRqOff0 : // fract32 raw SVF daming coefficient
    env_exp_set_off( &(voices[vid]->envRq), v);
    break;
  case eParamRqOn0 :
    env_exp_set_on( &(voices[vid]->envRq), v);
    if(voices[vid]->rqEnv == 0) {
      filter_svf_set_rq(&(voices[vid]->svf), v);
    }
    break;
  case eParamRqSus0 :
    env_exp_set_sus( &(voices[vid]->envRq), v);
    break;

  case eParamLow0 :	       
    filter_svf_set_low( &(voices[vid]->svf), v);
    break;
  case eParamHigh0 :	       
    filter_svf_set_high( &(voices[vid]->svf), v);
    break;
  case eParamBand0 :	       
    filter_svf_set_band( &(voices[vid]->svf), v);
    break;
  case eParamNotch0 :	       
    filter_svf_set_notch( &(voices[vid]->svf), v);
    break;

  case eParamSvfPre0 :
    set_param_svfpre(voices[vid], v);
    break;	
  case eParamFreqEnv0 :
    set_param_freqenv(voices[vid], v);
    break;	
  case eParamRqEnv0 :
    set_param_rqenv(voices[vid], v);
    break;		

  default:
    break;
  }
}
