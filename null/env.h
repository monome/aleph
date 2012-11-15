#ifndef _ENV_H_
#define _ENV_H_

// std
#include <math.h>
// aleph-common
#include "fix.h"
#include "fract32_emu.h"
// null
#include "types.h"

/// ASR envelope class
typedef struct _env_asr {
  ///// atk params
  fix16 aUp; // grow coefficient
  fract32 bUp; // decay coefficient
  fract32 rUp; // ramp increment
  fract32 cUp; // curve
  ///// rel params
  fix16 aDn; // grow coefficient
  fract32 bDn; // decay coefficient
  fract32 rDn; // ramp increment
  fract32 cDn; // curve
  //// current
  fract32 x; // current integrator value
  fract32 y; // current ramp value
  u8 state;      // current state
  fract32(*stateFP)(); // pointer to state-processing function
} env_asr;

// intialize at pre-allocated memory
extern void env_asr_init(env_asr* env);

// set attack duration in samples
extern void env_asr_set_atk_dur(env_asr* env, u32 dur);

// set attack curve in [-1, 1]
extern void env_asr_set_atk_shape(env_asr* env, fract32 shape);

// set release duration in samples
extern void env_asr_set_rel_dur(env_asr* env, u32 dur);

// set release curve in [-1, 1]
extern void env_asr_set_rel_shape(env_asr* env, fract32 shape);

// set gate
extern void env_asr_set_gate(env_asr* env, u8 gate);

// get the next value
extern fract32 env_asr_next(env_asr* env);

#endif
