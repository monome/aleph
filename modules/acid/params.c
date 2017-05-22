#include <string.h>
#include "acid.h"
#include "noise_acid.h"
//#include "env_int.h"
#include "env_exp.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h"
#include "libfixmath/fix16_fract.h"


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

static void module_set_dsyn_voice_param(u8 vid, u32 idx, ParamValue v);
static void module_set_monosynth_voice_param(u8 vid, u32 idx, ParamValue v);

// set parameter by value
void module_set_param(u32 idx, ParamValue v) {
  /// offset hack on parameter index
  if(idx < PARAM_VOICE_NPARAMS) {
    module_set_dsyn_voice_param(0, idx, v);
  }/*  else if (idx < 2 * PARAM_VOICE_NPARAMS) { */
  /*   module_set_voice_param(1, idx - PARAM_VOICE_NPARAMS, v); */
  /* } else if (idx < 3 * PARAM_VOICE_NPARAMS) { */
  /*   module_set_voice_param(2, idx - 2 * PARAM_VOICE_NPARAMS, v); */
  /* } else if (idx < 4 * PARAM_VOICE_NPARAMS) { */
  /*   module_set_voice_param(3, idx - 3 * PARAM_VOICE_NPARAMS, v); */
  /* } */ else {
    module_set_monosynth_voice_param(0, idx - DSYN_NPARAMS, v);
  }
}

//
static void module_set_dsyn_voice_param(u8 vid, u32 idx, ParamValue v) {
  switch(idx) {

  case dsynParamTrig : // trig bang
    drumsyn_voice_bang(voices[vid]);
    break;
    
  case dsynParamPreGain : // fract32 amp
    voices[vid]->noiseGain = v;    
    break;

  case dsynParamPostGain : // fract32 amp
    voices[vid]->postGain = v;    
    break;

  case dsynParamAmpAtkSlew : // fract32 raw 1pole coefficient
    voices[vid]->envAmp.adsr.attackTime = v;
    break;

  case dsynParamAmpSus : // fract32 amp
    voices[vid]->envAmp.adsr.sustainLevel = v;    
    break;
    
  case dsynParamAmpDecSlew : // fract32 raw 1pole coefficient
    voices[vid]->envAmp.adsr.decayTime = v;
    break;

  case dsynParamAmpRelSlew :
    voices[vid]->envAmp.adsr.releaseTime = v;
    break;

  case dsynParamAmpSusDur :
    voices[vid]->envAmp.sustainTime = v;
    break;


  case dsynParamFreqAtkSlew : // fract32 raw 1pole coefficient
    voices[vid]->envFreq.adsr.attackTime = v;
    break;

  case dsynParamFreqSus : // fract32 freq
    voices[vid]->envFreq.adsr.sustainLevel = v;    
    break;
    
  case dsynParamFreqDecSlew : // fract32 raw 1pole coefficient
    voices[vid]->envFreq.adsr.decayTime = v;
    break;

  case dsynParamFreqRelSlew :
    voices[vid]->envFreq.adsr.releaseTime = v;
    break;

  case dsynParamFreqSusDur :
    voices[vid]->envFreq.sustainTime = v;
    break;

  case dsynParamFreqOff : // fract32 raw SVF corner coefficient
    voices[vid]->freqOff = v;
    break;
  case dsynParamFreqOn: 
    voices[vid]->freqOn = v;
    break;

  case dsynParamRq :	       
    filter_svf_set_rq( &(voices[vid]->svf), v);
    break;
  case dsynParamLow :	       
    filter_svf_set_low( &(voices[vid]->svf), v);
    break;
  case dsynParamHigh :	       
    filter_svf_set_high( &(voices[vid]->svf), v);
    break;
  case dsynParamBand :	       
    filter_svf_set_band( &(voices[vid]->svf), v);
    break;
  case dsynParamNotch :	       
    filter_svf_set_notch( &(voices[vid]->svf), v);
    break;

  case dsynParamSvfPre :
    set_param_svfpre(voices[vid], v);
    break;	
  case dsynParamFreqEnv :
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
    for( i=0; i<dsynNumParams; ++i) {
      const int o = j * dsynNumParams;

      param_setup(o + dsynParamTrig, 0);

      param_setup(o + dsynParamPreGain, PARAM_AMP_6);
      param_setup(o + dsynParamPostGain, PARAM_AMP_6);
      param_setup(o + dsynParamAmpAtkSlew, SLEW_1MS);
      param_setup(o + dsynParamAmpDecSlew, SLEW_100MS);
      param_setup(o + dsynParamAmpRelSlew, SLEW_1S);
      param_setup(o + dsynParamAmpSusDur, 4800);
      param_setup( o + dsynParamFreqOff, PARAM_CUT_DEFAULT >> 2);
      param_setup( o + dsynParamFreqOn, PARAM_CUT_DEFAULT ); 
      param_setup( o + dsynParamFreqSus, PARAM_CUT_DEFAULT >> 1); 
      param_setup( o + dsynParamFreqAtkSlew, SLEW_1MS );
      param_setup( o + dsynParamFreqDecSlew, SLEW_100MS );
      param_setup( o + dsynParamFreqRelSlew, SLEW_1S );
      param_setup( o + dsynParamFreqSusDur, 2400 );

      param_setup( o + dsynParamRq, PARAM_RQ_DEFAULT );
      param_setup( o + dsynParamLow, PARAM_AMP_0 );
      param_setup( o + dsynParamHigh, 0 );
      param_setup( o + dsynParamBand, 0 );
      param_setup( o + dsynParamNotch, 0 );
      param_setup( o + dsynParamSvfPre, 1 );
      param_setup( o + dsynParamFreqEnv, 1 );
    }
  }
    for( j=0; j<MONOSYNTH_NVOICES; ++j) {
    for( i=0; i<monosynthNumParams; ++i) {
      const int o = j * dsynNumParams + DSYN_NPARAMS;

      param_setup(o + monosynthParamNoteTrigger, 0);
      param_setup(o + monosynthParamNoteHz, 440 << 16);
      param_setup(o + monosynthParamNoteTune, FIX16_ONE);
      param_setup(o + monosynthParamNoteLevel, FR32_MAX);
    }
  }
}
static void module_set_monosynth_voice_param(u8 vid, u32 idx, ParamValue v) {
  switch(idx) {
  case monosynthParamNoteTrigger :
    if (v > 0) {
      monosynthVoice_noteon(&mVoices[vid]);
    }
    else {
      monosynthVoice_noteoff(&mVoices[vid]);
    }
    break;

  case monosynthParamNoteHz :
    mVoices[vid].noteHz = v;
    mVoices[vid].noteOsc.freq = fix16_mul_fract(mVoices[vid].noteTune,
						mVoices[vid].noteHz);
    break;

  case monosynthParamNoteTune :
    mVoices[vid].noteTune = v;
    mVoices[vid].noteOsc.freq = fix16_mul_fract(mVoices[vid].noteTune,
						mVoices[vid].noteHz);
    break;

  case monosynthParamNoteLevel :
    // FIXME add some mixing
    break;

  default :
    break;
  }
}
