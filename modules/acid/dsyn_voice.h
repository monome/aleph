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
  fract32 noise;
  fract32 noiseGain;

  env_trig_adsr envAmp;
  
  env_trig_adsr envFreq;
  fract32 freqOn;
  fract32 freqOff;

  // filter pre/post amplitude
  u8 svfPre;
  
  // env->frequency flag
  // if false, use freqOn as value
  u8 freqEnv;

  // env->rq flag
  // if false, use freqOn as value
  u8 rqEnv;

  // gate!
  u8 gate;

  // FIXME don't do this mixing inside DSP object
  fract32 postGain;
} drumsynVoice;

extern void drumsyn_voice_init(drumsynVoice* voice);
extern void drumsyn_voice_bang(drumsynVoice* vp);
extern fract32 drumsyn_voice_next(drumsynVoice* voice);
#endif
