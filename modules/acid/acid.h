#ifndef _ALEPH_ACID_H_
#define _ALEPH_ACID_H_

#include "dsyn_voice.h"
#include "monosynth_voice.h"
#include "params.h"
/// testing
// #define DRUMSYN_NOENV 1


extern drumsynVoice* voices[DRUMSYN_NVOICES];
extern monosynthVoice mVoices[MONOSYNTH_NVOICES];

extern fract32 dsynLevels[DRUMSYN_NVOICES];
extern fract32 monosynthLevels[MONOSYNTH_NVOICES];

//////
///
/// TESTing 
// envelope is nuts? trying just a raw integrator
//extern filter_1p_fr32 ampLp;
//
/////


#endif // h guard
