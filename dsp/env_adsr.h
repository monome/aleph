#ifndef ENV_ADSR_H
#define ENV_ADSR_H
#include "types.h"

enum adsrStates {
  adsrAttack,
  adsrDecay,
  adsrRelease,
};

typedef struct env_adsr {
  char envState;
  fract32 envOut;
  fract32 overshoot;
  fract32 attackTime;
  fract32 decayTime;
  fract32 sustainLevel;
  fract32 releaseTime;
} env_adsr;

void env_adsr_init(env_adsr *env);

void env_adsr_press (env_adsr *env);

void env_adsr_release (env_adsr *env);

fract32 env_adsr_next(env_adsr *env);

typedef struct env_adsr_16 {
  char envState;
  fract16 envOut;
  fract16 overshoot;
  fract16 attackTime;
  fract16 decayTime;
  fract16 sustainLevel;
  fract16 releaseTime;
} env_adsr_16;

void env_adsr_init_16(env_adsr_16 *env);

void env_adsr_press_16 (env_adsr_16 *env);

void env_adsr_release_16 (env_adsr_16 *env);

fract16 env_adsr_next_16(env_adsr_16 *env);

#endif
