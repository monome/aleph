#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "env.h"
#include "filter_svf.h"
#include "noise.h"

#define DRUMSYN_NVOICES 1

typedef struct _drumsynVoice {
  filter_svf* svf;
  u8 gate;
  fract32 amp;
  env_asr* envAmp;
  env_asr* envFreq;
  env_asr* envRes;
  //  fract32 envOffAmp; // offset 
  //  fract32 envMulAmp; // multiplier
  fract32 envAddFreq; // offset 
  fract32 envMulFreq; // multiplier
  fract32 envAddRes; // offset 
  fract32 envMulRes; // multiplier
  lcprng* rngH;
  lcprng* rngL;
} drumsynVoice;

extern drumsynVoice* voices[DRUMSYN_NVOICES];

#endif // h guard
