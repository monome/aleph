#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "env_int.h"
#include "filter_svf.h"
#include "noise.h"

/// testing
#define DRUMSYN_NOENV 1

#define DRUMSYN_NVOICES 1

typedef struct _drumsynVoice {
  filter_svf* svf;
  lcprng* rngH;
  lcprng* rngL;
  fract32 amp;
#ifdef DRUMSYN_NOENV
  // integrators
  filter_1p_fr32 lpAmp;
  filter_1p_fr32 lpFreq;
  filter_1p_fr32 lpRq;
  fract32 freqOn;
  fract32 freqOff;
  fract32 rqOn;
  fract32 rqOff;
  fract32 ampSlewUp;
  fract32 ampSlewDown;
  fract32 freqSlewUp;
  fract32 freqSlewDown;
  fract32 rqSlewUp;
  fract32 rqSlewDown;
#else
  // envelopes
  env_int* envAmp;
  env_int* envFreq;
  env_int* envRq;
  //  fract32 envOffAmp;
  //  fract32 envMulAmp;
  /// rrg
  fract32 envEndFreq;
  fract32 envStartRq;
  fract32 envEndRq;
#endif
  u8 gate;
} drumsynVoice;

extern drumsynVoice* voices[DRUMSYN_NVOICES];

//////
///
/// TESTing 
// envelope is nuts? trying just a raw integrator
extern filter_1p_fr32 ampLp;
//
/////


#endif // h guard
