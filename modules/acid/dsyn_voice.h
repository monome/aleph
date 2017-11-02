#ifndef _ALEPH_DSYN_VOICE_H_
#define _ALEPH_DSYN_VOICE_H_

#include "env_trig.h"
#include "filter_svf.h"
#include "filter_2p.h"
#include "noise_acid.h"

typedef struct _drumsynVoice {
  // filter
  filter_svf svf;

  // noise
  u8 noiseReset; // flag to reset white noise on bang
  fract32 noise; // state variable for noise generator
  fract32 noiseGain; // noise gain into svf filter

  env_trig_adsr envAmp; // amplitude envelope
  
  env_trig_adsr envFreq; // freq envelope
  fract32 freqOn; // frequency origin
  fract32 freqOff; // frequency @ attack peak

  // flag to filter pre/post amplitude
  u8 svfPre;
  
  // env->frequency flag
  // if false, use freqOn as value
  u8 freqEnv;

} drumsynVoice;

extern void drumsyn_voice_init(drumsynVoice* voice);
extern void drumsyn_voice_bang(drumsynVoice* vp);
extern fract32 drumsyn_voice_next(drumsynVoice* voice);
#endif
