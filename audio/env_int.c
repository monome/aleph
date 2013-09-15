/* env.c
   
   simple ASR envelope class based on dual integrators.

 */

#include "env_int.h" 

// states
enum {
  envStateAtk,
  envStateSus,
  envStateRel,
  envStateOff,
};

//----------------------------------
//----- -static function declarations

// next-value functions
static fract32 next_atk(env_int* env);
static fract32 next_sus_trig0(env_int* env);
static fract32 next_sus_trig1(env_int* env);
static fract32 next_atk(env_int* env);
static fract32 next_off(env_int* env);

// state-change functions
static void state_atk(env_int* env);
// retrigger
static void state_atk_reset(env_int* env);
static void state_sus(env_int* env);
static void state_rel(env_int* env);
static void state_off(env_int* env);

//------------------------------------
//---- extern function definitions

// initialize at pre-allocated memory
extern void env_int_init(env_int* env) {
  filter_1p_fr32_init( &(env->intAtk), 0x3fffffff);
  filter_1p_fr32_init( &(env->intRel), 0);
  env->scale = 0x3fffffff;
  env->susCount = 0;
  env->susDur = 1;
  env->gate = 0;
  env->trig = 0;
  env->state = envStateOff;
  env->stateFP = &next_off;
}

// get the next value
fract32 env_int_next(env_int* env) {
  return (*(env->stateFP))(env);
}

// set gate
extern void env_int_set_gate(env_int* env, u8 g) {
  if(g == env->gate) {
    return;
  }
  env->gate = g;
  ///// trigger mode: always reset
  if(env->trig) {
    state_atk_reset(env);
    return;
  }
  //// not trigger mode, don't reset
  // release
  if(g) {
    state_atk(env);
  } else {
    state_rel(env);
  }
}

// trigger (oneshot)
extern void env_int_set_trig(env_int* env, u8 trig) {
  env->trig = trig;
  if(env->state == envStateSus) {
    if(trig) {
      env->stateFP =  &next_sus_trig1;
    } else {
      env->stateFP = &next_sus_trig0;
    }
  }
}
// set scale
extern void env_int_set_scale(env_int* env, fract32 scale) {
  env->scale = scale;
  if(env->state == envStateAtk) {
    filter_1p_fr32_in(&(env->intAtk), scale);
  } else if(env->state == envStateRel) {
    if(env->intRel.y > scale) {
      env->intRel.y = scale;
    }
  }
}

// set attack integrator coefficient
extern void env_int_set_atk_coeff(env_int* env, fract32 c) {
  filter_1p_fr32_set_coeff(&(env->intAtk), c);
}

// set release integrator coeff
extern void env_int_set_rel_coeff(env_int* env, fract32 c) {
  filter_1p_fr32_set_coeff(&(env->intRel), c);
}

// set sustain duration (ignored in gated mode)
extern void env_int_set_sus_dur(env_int* env, u32 samps) {
  env->susDur = samps;
}

//-------------------------------------
//---- static function definitions

// next-value functions
static fract32 next_atk(env_int* env) {
  fract32 val;
  val = filter_1p_fr32_next( &(env->intAtk) );
  if( (env->intAtk).sync) { 
    state_sus(env);
  }
  return val;
}

static fract32 next_sus_trig0(env_int* env) {
  return env->scale;
}

static fract32 next_sus_trig1(env_int* env) {
  env->susCount++;
  if(env->susCount == env->susDur) {
    state_rel(env);
  }
  return env->scale;
}

static fract32 next_rel(env_int* env) {
  fract32 val;
  val = filter_1p_fr32_next( &(env->intRel) );
  if( (env->intRel).sync) { 
    state_off(env);
  }
  return val;

}

static fract32 next_off(env_int* env) {
  return 0;
}

// state-change functions
static void state_atk(env_int* env) {
  switch(env->state) {
  case envStateOff : 
    // hard-reset attack integrator to zero
    env->intAtk.y = 0;
    filter_1p_fr32_in(&(env->intAtk), env->scale);
    break;
  case envStateRel : 
    // hard-reset attack integrator to release integrator value
    env->intAtk.y = env->intRel.y;
    filter_1p_fr32_in(&(env->intAtk), env->scale);    
    break;
  case envStateAtk : 
    ;;
    break;
  case envStateSus : 
    ;;
    break;
  }
  env->stateFP = &next_atk;
  env->state = envStateAtk;
}

static void state_atk_reset(env_int* env) {
  // FIXME: hard reset for now
  env->intAtk.y = 0;
  filter_1p_fr32_in(&(env->intAtk), env->scale);
  env->stateFP = &next_atk;
  env->state = envStateAtk;
}

static void state_sus(env_int* env) {
  env->susCount = 0;
  if(env->trig) {
    env->stateFP = &next_sus_trig1;
  } else {
    env->stateFP = &next_sus_trig0;
  }
  env->state = envStateSus;
}

static void state_rel(env_int* env) {
  switch(env->state) {
  case envStateOff : 
    ;;
    break;
  case envStateRel : 
    ;;
    break;
  case envStateAtk : 
    // hard-reset release integrator to attack integrator value
    env->intRel.y = env->intAtk.y;
    filter_1p_fr32_in(&(env->intRel), 0);    
    break;
  case envStateSus : 
    // hard-reset release integrator to scale
    env->intRel.y = env->scale;
    filter_1p_fr32_in(&(env->intRel), 0);    
    break;
  }
  env->stateFP = &next_rel;
  env->state = envStateRel;
}

static void state_off(env_int* env) {
  env->stateFP = &next_off;
  env->state = envStateOff;
}


