/* env_int.h

   simple ASR envelope class based on two integrators.
   no curve control
 */

#ifndef _ALEPH_AUDIO_ENV_INT_H_
#define _ALEPH_AUDIO_ENV_INT_H_

#include "filter_1p.h"
#include "types.h"

/// ASR envelope class
/// can be used in gated mode or triggered mode
typedef struct _env_int {
  // attack
  filter_1p_fr32 intAtk;
  // release integrator
  filter_1p_fr32 intRel;
  // scaling value (more efficient to do here than multiply later)
  fract32 scale;
  // sustain counter
  u32 susCount;
  // sustain duration (in samples);
  u32 susDur;
  // gate flag
  u8 gate;
  // state value
  u8 state;
  // trigger mode
  u8 trig;
  // next-value function for current state
  fract32 (*stateFP)();
} env_int;

// initialize at pre-allocated memory
extern void env_int_init(env_int* env);

// set attack integrator coeffficnet
extern void env_int_set_atk_coeff(env_int* env, fract32 c);

// set release duration integrator coefficient
extern void env_int_set_rel_coeff(env_int* env, fract32 c);

// set sustain duration in samples (ignored in gated mode)
extern void env_int_set_sus_dur(env_int* env, u32 dur);

// set gate
extern void env_int_set_gate(env_int* env, u8 gate);

// trigger (oneshot)
extern void env_int_set_trig(env_int* env, u8 gate);

// set scale
extern void env_int_set_scale(env_int* env, fract32 scale);

// get the next value
extern fract32 env_int_next(env_int* env);

#endif
