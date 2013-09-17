#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "env_exp.h"
#include "filter_svf.h"
#include "noise.h"

/// testing
#define DRUMSYN_NOENV 1

#define DRUMSYN_NVOICES 4

typedef struct _drumsynVoice {
  // filter
  filter_svf svf;
  // noise generation
  lcprng rngH;
  lcprng rngL;

  // 2nd order highpass for noise
  //// TODO
  //  filter_2p hipass;
 
  // envelopes
  env_exp envAmp;
  env_exp envFreq;
  env_exp envRq;
// gate!
//  u8 gate;
} drumsynVoice;

extern drumsynVoice voices[DRUMSYN_NVOICES];


#endif // h guard
