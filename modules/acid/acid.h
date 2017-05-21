#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "dsyn_voice.h"
/// testing
// #define DRUMSYN_NOENV 1

#define DRUMSYN_NVOICES 4

extern drumsynVoice* voices[DRUMSYN_NVOICES];

//////
///
/// TESTing 
// envelope is nuts? trying just a raw integrator
//extern filter_1p_fr32 ampLp;
//
/////


#endif // h guard
