#include "env_adsr.h"
#include "fract_math.h"
#include "fix.h"
#include "ricks_tricks.h"

void env_adsr_init(env_adsr *env) {
  env->envState=adsrRelease;
  env->envOut = 0;
  env->overshoot = FR32_MAX / 10;
  env->attackTime = SLEW_10MS;
  env->decayTime = SLEW_100MS;
  env->sustainLevel = FR32_MAX >> 2;
  env->releaseTime = SLEW_1S;
}

void env_adsr_press (env_adsr *env) {
  env->envState = adsrAttack;
}

void env_adsr_release (env_adsr *env) {
  env->envState = adsrRelease;
}

fract32 env_adsr_next(env_adsr *env) {
  switch(env->envState) {
  case adsrAttack :
    normalised_logSlew(&(env->envOut), FR32_MAX, env->attackTime);
    if(env->envOut > FR32_MAX - env->overshoot) {
      env->envState = adsrDecay;
    }
    break;
  case adsrDecay :
    normalised_logSlew(&(env->envOut), env->sustainLevel, env->decayTime);
    break;
  case adsrRelease :
    normalised_logSlew(&(env->envOut), 0, env->releaseTime);
    break;
  }
  return env->envOut;
}



void env_adsr_init_16(env_adsr_16 *env) {
  env->envState=adsrRelease;
  env->envOut = 0;
  env->overshoot = FR16_MAX / 10;
  env->attackTime = SLEW_10MS_16;
  env->decayTime = SLEW_100MS_16;
  env->sustainLevel = FR16_MAX >> 2;
  env->releaseTime = SLEW_1S_16;
}

void env_adsr_press_16 (env_adsr_16 *env) {
  env->envState = adsrAttack;
}

void env_adsr_release_16 (env_adsr_16 *env) {
  env->envState = adsrRelease;
}

fract16 env_adsr_next_16(env_adsr_16 *env) {
  switch(env->envState) {
  case adsrAttack :
    normalised_logSlew_16(&(env->envOut), FR16_MAX, env->attackTime);
    if(env->envOut > FR16_MAX - env->overshoot) {
      env->envState = adsrDecay;
    }
    break;
  case adsrDecay :
    normalised_logSlew_16(&(env->envOut), env->sustainLevel, env->decayTime);
    break;
  case adsrRelease :
    normalised_logSlew_16(&(env->envOut), 0, env->releaseTime);
    break;
  }
  return env->envOut;
}
