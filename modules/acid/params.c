#include <string.h>
#include "acid.h"
#include "noise_acid.h"
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
    filter_svf_set_coeff(&(vp->svf), vp->freqOn);
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

  case eParamTrig0 : // trig bang
    drumsyn_voice_bang(voices[vid]);
    break;
    
  case eParamPreGain0 : // fract32 amp
    voices[vid]->noiseGain = v;    
    break;

  case eParamPostGain0 : // fract32 amp
    voices[vid]->postGain = v;    
    break;

  case eParamAmpAtkSlew0 : // fract32 raw 1pole coefficient
    voices[vid]->envAmp.adsr.attackTime = v;
    break;

  case eParamAmpSus0 : // fract32 amp
    voices[vid]->envAmp.adsr.sustainLevel = v;    
    break;
    
  case eParamAmpDecSlew0 : // fract32 raw 1pole coefficient
    voices[vid]->envAmp.adsr.decayTime = v;
    break;

  case eParamAmpRelSlew0 :
    voices[vid]->envAmp.adsr.releaseTime = v;
    break;

  case eParamAmpSusDur0 :
    voices[vid]->envAmp.sustainTime = v;
    break;


  case eParamFreqAtkSlew0 : // fract32 raw 1pole coefficient
    voices[vid]->envFreq.adsr.attackTime = v;
    break;

  case eParamFreqSus0 : // fract32 freq
    voices[vid]->envFreq.adsr.sustainLevel = v;    
    break;
    
  case eParamFreqDecSlew0 : // fract32 raw 1pole coefficient
    voices[vid]->envFreq.adsr.decayTime = v;
    break;

  case eParamFreqRelSlew0 :
    voices[vid]->envFreq.adsr.releaseTime = v;
    break;

  case eParamFreqSusDur0 :
    voices[vid]->envFreq.sustainTime = v;
    break;

  case eParamFreqOff0 : // fract32 raw SVF corner coefficient
    voices[vid]->freqOff = v;
    break;
  case eParamFreqOn0: 
    voices[vid]->freqOn = v;
    break;

  case eParamRq0 :	       
    filter_svf_set_rq( &(voices[vid]->svf), v);
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

  default:
    break;
  }
}


#include "ricks_tricks.h"
/// setup default values
void params_default(void) {
  int i, j;
  for( j=0; j<DRUMSYN_NVOICES; ++j) {
    for( i=0; i<PARAM_VOICE_NPARAMS; ++i) {
      const int o = j * PARAM_VOICE_NPARAMS;

      param_setup(o + eParamTrig0, 0);

      param_setup(o + eParamPreGain0, PARAM_AMP_6);
      param_setup(o + eParamPostGain0, PARAM_AMP_6);
      param_setup(o + eParamAmpAtkSlew0, SLEW_1MS);
      param_setup(o + eParamAmpDecSlew0, SLEW_100MS);
      param_setup(o + eParamAmpRelSlew0, SLEW_1S);
      param_setup(o + eParamAmpSusDur0, 4800);
      param_setup( o + eParamFreqOff0, PARAM_CUT_DEFAULT >> 2);
      param_setup( o + eParamFreqOn0, PARAM_CUT_DEFAULT ); 
      param_setup( o + eParamFreqSus0, PARAM_CUT_DEFAULT >> 1); 
      param_setup( o + eParamFreqAtkSlew0, SLEW_1MS );
      param_setup( o + eParamFreqDecSlew0, SLEW_100MS );
      param_setup( o + eParamFreqRelSlew0, SLEW_1S );
      param_setup( o + eParamFreqSusDur0, 2400 );

      param_setup( o + eParamRq0, PARAM_RQ_DEFAULT );
      param_setup( o + eParamLow0, PARAM_AMP_0 );
      param_setup( o + eParamHigh0, 0 );
      param_setup( o + eParamBand0, 0 );
      param_setup( o + eParamNotch0, 0 );
      param_setup( o + eParamSvfPre0, 1 );
      param_setup( o + eParamFreqEnv0, 1 );
    }
  }
}
