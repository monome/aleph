#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "env_exp.h"
#include "filter_svf.h"
#include "filter_2p.h"
#include "noise.h"

/// testing
// #define DRUMSYN_NOENV 1

#define DRUMSYN_NVOICES 4

typedef struct _drumsynVoice {
  // filter
  filter_svf svf;
  // noise generation
  lcprng rngH;
  lcprng rngL;

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

extern drumsynVoice* voices[DRUMSYN_NVOICES];

//////
///
/// TESTing 
// envelope is nuts? trying just a raw integrator
//extern filter_1p_fr32 ampLp;
//
/////


#endif // h guard
