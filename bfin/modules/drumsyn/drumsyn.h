#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "env_exp.h"
#include "filter_svf.h"
#include "noise.h"

/// testing
#define DRUMSYN_NOENV 1

#define DRUMSYN_NVOICES 1

typedef struct _drumsynVoice {
  // filter
  filter_svf svf;
  // noise generation
  lcprng rngH;
  lcprng rngL;

  // 2nd order highpass for noise
  //// TODO
  //  filter_2p hipass;
  
  env_exp envAmp;
  env_exp envFreq;
  env_exp envRq;

  /*
  // amplitude
  fract32 amp;
  // integrators
  filter_1p_fr32 lpAmp;
  filter_1p_fr32 lpFreq;
  filter_1p_fr32 lpRq;
  // endpoints
  fract32 freqOn;
  fract32 freqOff;
  fract32 rqOn;
  fract32 rqOff;
  // slews
  fract32 ampSlewUp;
  fract32 ampSlewDown;
  fract32 freqSlewUp;
  fract32 freqSlewDown;
  fract32 rqSlewUp;
  fract32 rqSlewDown;
*/

  // sustain duration in samples
  /*
  u32 ampDur;
  u32 freqDur;
  u32 rqDur;
  */
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
