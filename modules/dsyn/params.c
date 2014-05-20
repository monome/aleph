#include <string.h>
#include "dsyn.h"
#include "noise_dsyn.h"
//#include "env_int.h"
#include "env_exp.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 


// initial param set
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


static void set_param_gate(drumsynVoice* vp, int i, s32 val) {
  if(val > 0) { 
    // re-seed noise generator to known value so "notes" are consistent
    //    lcprng_reset(&(vp->rngH), 0xDEADFACE);
    dsyn_noise_reset(i);
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
    set_param_gate(voices[vid], vid, v);
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

    /// RQ: incoming param is 16.16, [0, 1)
    /// target is 2.30, [0, 2)
  case eParamRqOff0 : 
    env_exp_set_off( &(voices[vid]->envRq), v << 14);
    break;
  case eParamRqOn0 :
    env_exp_set_on( &(voices[vid]->envRq), v << 14);
    if(voices[vid]->rqEnv == 0) {
      filter_svf_set_rq(&(voices[vid]->svf), v << 14);
    }
    break;
  case eParamRqSus0 :
    env_exp_set_sus( &(voices[vid]->envRq), v << 14);
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


/// setup default values
void params_default(void) {
  int i, j;
  for( j=0; j<4; ++j) {
    for( i=0; i<PARAM_VOICE_NPARAMS; ++i) {
      const int o = j * PARAM_VOICE_NPARAMS;

      param_setup(o + eParamTrig0, 1);

      param_setup(o + eParamAmp0, PARAM_AMP_6);
      param_setup(o + eParamAmpAtkSlew0, PARAM_SLEW_1MS);
      param_setup(o + eParamAmpDecSlew0, PARAM_SLEW_100MS);
      param_setup(o + eParamAmpRelSlew0, PARAM_SLEW_1S);
      param_setup(o + eParamAmpSusDur0, 4800);
      param_setup( o + eParamFreqOff0, 	PARAM_CUT_DEFAULT >> 2);
      param_setup( o + eParamFreqOn0, 	PARAM_CUT_DEFAULT ); 
      param_setup( o + eParamFreqSus0, 	PARAM_CUT_DEFAULT >> 1); 
      param_setup( o + eParamFreqAtkSlew0, 	PARAM_SLEW_1MS );
      param_setup( o + eParamFreqDecSlew0, 	PARAM_SLEW_100MS );
      param_setup( o + eParamFreqRelSlew0, 	PARAM_SLEW_1S );
      param_setup( o + eParamFreqSusDur0, 	2400 );

      param_setup( o + eParamRqOff0, 	PARAM_RQ_DEFAULT );
      param_setup( o + eParamRqOn0, 	PARAM_RQ_DEFAULT );
      param_setup( o + eParamRqSus0, 	PARAM_RQ_DEFAULT );
      param_setup( o + eParamRqAtkSlew0, 	PARAM_SLEW_1MS );
      param_setup( o + eParamRqDecSlew0, 	PARAM_SLEW_100MS );
      param_setup( o + eParamRqRelSlew0, 	PARAM_SLEW_1S );
      param_setup( o + eParamRqSusDur0, 	1200 );
      param_setup( o + eParamLow0, 	PARAM_AMP_0 );
      param_setup( o + eParamHigh0, 	0 );
      param_setup( o + eParamBand0, 	0 );
      param_setup( o + eParamNotch0, 	0 );
      param_setup( o + eParamSvfPre0, 	1 );
      param_setup( o + eParamFreqEnv0, 	1 );
      param_setup( o + eParamRqEnv0, 	1 );
    }
  }
}
