/* env_exp.h

   ADSR envelope class based on exponential integrators.
 */

#ifndef _ALEPH_AUDIO_ENV_EXP_H_
#define _ALEPH_AUDIO_ENV_EXP_H_

#include "filter_1p.h"
#include "types.h"

/// ADSR envelope class
/// can be used in gated mode or triggered mode
typedef struct _env_exp {
  // attack lowpass
  filter_1p_lo lpAtk;
  // decay integrator
  filter_1p_lo lpDec;
  // release integrator
  filter_1p_lo lpRel;
    // off value
  fract32 valOff;
  // on value
  fract32 valOn;
  // sustain value
  fract32 valSus;
  // sustain counter (trigger mode)
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
} env_exp;

// initialize at pre-allocated memory
extern void env_exp_init(env_exp* env);
// set attack integrator coefficient
extern void env_exp_set_atk_slew(env_exp* env, fract32 c);
// set decay integrator coefficient
extern void env_exp_set_dec_slew(env_exp* env, fract32 c);
// set release duration integrator coefficient
extern void env_exp_set_rel_slew(env_exp* env, fract32 c);
// set sustain duration in samples (ignored in gated mode)
extern void env_exp_set_sus_dur(env_exp* env, u32 dur);
// set gate
extern void env_exp_set_gate(env_exp* env, u8 gate);
// trigger (oneshot)
extern void env_exp_set_trig(env_exp* env, u8 gate);
// set on value
extern void env_exp_set_on(env_exp* env, fract32 val);
// set off value
extern void env_exp_set_off(env_exp* env, fract32 val);
// set sustain value
extern void env_exp_set_sus(env_exp* env, fract32 val);
// get the next value
extern fract32 env_exp_next(env_exp* env);

#endif
