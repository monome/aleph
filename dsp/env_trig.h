#ifndef ENV_TRIG_H
#define ENV_TRIG_H
#include "env_adsr.h"

typedef struct env_trig_adsr {
  env_adsr adsr;
  fract32 sustainTime;
  fract32 countdown;
} env_adsr;
extern void env_trig_adsr_init(env_adsr *env);
extern void env_trig_adsr_bang (env_adsr *env);
extern fract32 env_trig_adsr_next(env_adsr *env);

typedef struct env_trig_adsr_16 {
  env_adsr_16 adsr;
  fract32 sustainTime;
  fract32 countdown;
} env_adsr;
extern void env_trig_adsr_16_init(env_adsr *env);
extern void env_trig_adsr_16_bang (env_adsr *env);
extern fract32 env_trig_adsr_16_next(env_adsr *env);

#endif
