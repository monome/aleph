#ifndef _ALEPH_DSYN_VOICE_H_
#define _ALEPH_DSYN_VOICE_H_

#include "env_exp.h"
#include "filter_svf.h"
#include "filter_2p.h"
#include "noise_acid.h"

typedef struct _drumsynVoice {
  // filter
  filter_svf svf;

  fract32 noise;

  // 2nd order highpass for noise
  filter_2p_hi hipass;
  
  env_exp envAmp;
  env_exp envFreq;
  env_exp envRq;

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
} drumsynVoice;

extern void drumsyn_voice_init(drumsynVoice* voice);
extern fract32 drumsyn_voice_next(drumsynVoice* voice);
#endif
