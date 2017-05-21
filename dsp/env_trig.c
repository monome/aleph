// trig adsr is an auto-off adsr - set the param sustainTime to
// control notelength, rather than sending a separate release event

#include "env_trig.h"

void env_trig_adsr_init(env_trig_adsr *env) {
  env_adsr_init(&env->adsr);
  env->sustainTime = 48 * 50;
  env->countdown = 0;
}

void env_trig_adsr_bang (env_trig_adsr *env) {
  env->countdown = env->sustainTime;
  env_adsr_press(&env->adsr);
}

fract32 env_trig_adsr_next(env_trig_adsr *env) {
  if (env->countdown > 0) {
    env->countdown--;
  }
  else {
    env_adsr_release(&env->adsr);
  }
  return env_adsr_next(&env->adsr);
}


void env_trig_adsr_16_init(env_trig_adsr_16 *env) {
  env_adsr_16_init(&env->adsr);
  env->sustainTime = 48 * 50;
  env->countdown = 0;
}

void env_trig_adsr_16_bang (env_trig_adsr_16 *env) {
  env->countdown = env->sustainTime;
  env_adsr_16_press(&env->adsr);
}

fract32 env_trig_adsr_16_next(env_trig_adsr_16 *env) {
  if (env->countdown > 0) {
    env->countdown--;
  }
  else {
    env_adsr_16_release(&env->adsr);
  }
  return env_adsr_16_next(&env->adsr);
}

