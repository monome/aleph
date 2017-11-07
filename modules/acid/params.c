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
  int off;
  if(idx < DSYN_NPARAMS) {
    off = 0;
    while (idx >= dsynNumParams) {
      idx -= dsynNumParams;
      off++;
    }
    module_set_dsyn_voice_param(off, idx, v);
  } else {
    off = 0;
    idx -= DSYN_NPARAMS;
    while (idx >= monosynthNumParams) {
      idx -= monosynthNumParams;
      off++;
    }
    module_set_monosynth_voice_param(off, idx, v);
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
    dsynLevels[vid] = v;
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
    filter_svf_set_rq( &(voices[vid]->svf), v << 14);
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
      param_setup( o + dsynParamFreqOff, PARAM_CUT_DEFAULT >> 1);
      param_setup( o + dsynParamFreqOn, PARAM_CUT_DEFAULT ); 
      param_setup( o + dsynParamFreqSus, PARAM_CUT_DEFAULT);
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
      const int o = j * monosynthNumParams + DSYN_NPARAMS;

      param_setup(o + monosynthParamNoteTrigger, 0);
      param_setup(o + monosynthParamNoteHz, 440 << 16);
      param_setup(o + monosynthParamNoteTune, FIX16_ONE);
      param_setup( o + monosynthParamOscFlava, 0 );

      param_setup(o + monosynthParamAmpAttack, SLEW_1MS);
      param_setup(o + monosynthParamAmpDecay, SLEW_100MS);
      param_setup(o + monosynthParamAmpSustain, PARAM_AMP_6);
      param_setup(o + monosynthParamAmpRelease, SLEW_1S);
      param_setup(o + monosynthParamNoteLevel, FR32_MAX);

      param_setup(o + monosynthParamFiltAttack, SLEW_1MS);
      param_setup(o + monosynthParamFiltDecay, SLEW_100MS);
      param_setup(o + monosynthParamFiltSustain, PARAM_AMP_6);
      param_setup(o + monosynthParamFiltRelease, SLEW_1S);
      param_setup( o + monosynthParamFreqOff, PARAM_CUT_DEFAULT >> 2);
      param_setup( o + monosynthParamFreqOn, PARAM_CUT_DEFAULT << 4);

      param_setup( o + monosynthParamRq, PARAM_RQ_DEFAULT );
      param_setup( o + monosynthParamFiltType, 7 );
      param_setup( o + monosynthParamDist, 4 );

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
  case monosynthParamOscFlava :
    mVoices[vid].oscFlavour = v;
    break;

  case monosynthParamAmpAttack : // fract32 raw 1pole coefficient
    mVoices[vid].ampEnv.attackTime = v;
    break;
  case monosynthParamAmpDecay : // fract32 raw 1pole coefficient
    mVoices[vid].ampEnv.decayTime = v;
    break;
  case monosynthParamAmpSustain : // fract32 freq
    mVoices[vid].ampEnv.sustainLevel = v;
    break;
  case monosynthParamAmpRelease :
    mVoices[vid].ampEnv.releaseTime = v;
    break;
  case monosynthParamNoteLevel :
    monosynthLevels[vid] = v;
    break;

  case monosynthParamFiltAttack : // fract32 raw 1pole coefficient
    mVoices[vid].filterEnv.attackTime = v;
    break;
  case monosynthParamFiltDecay : // fract32 raw 1pole coefficient
    mVoices[vid].filterEnv.decayTime = v;
    break;
  case monosynthParamFiltSustain : // fract32 freq
    mVoices[vid].filterEnv.sustainLevel = v;
    break;
  case monosynthParamFiltRelease :
    mVoices[vid].filterEnv.releaseTime = v;
    break;

  case monosynthParamFreqOff : // fract32 raw SVF corner coefficient
    mVoices[vid].freqOff = v;
    break;
  case monosynthParamFreqOn:
    mVoices[vid].freqOn = v;
    break;

  case monosynthParamRq :
    filter_svf_set_rq( &(mVoices[vid].svf), v << 14);
    break;

  case monosynthParamFiltType :
    mVoices[vid].filtType = v - 7;
    break;

  case monosynthParamDist :
    mVoices[vid].clipFlavour = v - 4;
    break;


  default :
    break;
  }
}
