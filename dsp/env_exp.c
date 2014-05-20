/* env_exp
   
   simple ADSR envelope class based on exponential integrators.

*/

#include "env_exp.h" 

// states
enum {
  envStateAtk,
  envStateDec,
  envStateSus,
  envStateRel,
  envStateOff,
};  

//----------------------------------
//----- -static function declarations

// next-value functions
static fract32 next_atk(env_exp* env);
static fract32 next_dec(env_exp* env);
static fract32 next_sus_trig0(env_exp* env);
static fract32 next_sus_trig1(env_exp* env);
static fract32 next_rel(env_exp* env);
static fract32 next_off(env_exp* env);

// state-change functions
static void state_atk(env_exp* env);
// retrigger
static void state_atk_reset(env_exp* env);
static void state_dec(env_exp* env);
static void state_sus(env_exp* env);
static void state_rel(env_exp* env);
static void state_off(env_exp* env);

//------------------------------------
//---- extern function definitions

// initialize at pre-allocated memory
void env_exp_init(env_exp* env) {
  env->valOn =  0x3fffffff;
  env->valSus = 0x1fffffff;
  env->valOff = 0;
  env->susCount = 0;
  env->susDur = 16;
  env->gate = 0;
  env->trig = 0;
  env->state = envStateOff;
  env->stateFP = &next_off;
  filter_1p_lo_init( &(env->lpAtk), env->valOff);
  filter_1p_lo_init( &(env->lpDec), env->valOn);
  filter_1p_lo_init( &(env->lpRel), env->valSus);
}

// get the next value
fract32 env_exp_next(env_exp* env) {
  return (*(env->stateFP))(env);
}

// set gate
void env_exp_set_gate(env_exp* env, u8 g) {
  /// ... this ignores retriggers. maybe a desirable option.
  /* if(g == env->gate) { */
  /*   return; */
  /* } */
  env->gate = g;
  if( g > 0 ) {
    if (env->trig) {
      state_atk_reset(env);
    } else { 
      state_atk(env); 
    }
  } else {
    state_rel(env);
  }
}

// trigger (oneshot)
void env_exp_set_trig(env_exp* env, u8 trig) {
  env->trig = trig;
  if(env->state == envStateSus) {
    if(trig) {
      env->stateFP =  &next_sus_trig1;
    } else {
      env->stateFP = &next_sus_trig0;
    }
  }
}

// set on value
void env_exp_set_on(env_exp* env, fract32 v) {
  //  env->scale = scale;
  env->valOn = v; // omg
  if(env->state == envStateAtk) {
    filter_1p_lo_in(&(env->lpAtk), v);
  } else {
    // bah
  }
}

// set off value
void env_exp_set_off(env_exp* env, fract32 v) {
  env->valOff = v;
  if(env->state == envStateRel) {
    filter_1p_lo_in(&(env->lpRel), v);
  } else {
    // bah
  }
}

// set sustain value
void env_exp_set_sus(env_exp* env, fract32 v) {
  env->valSus = v;
  if(env->state == envStateRel) {
    filter_1p_lo_in(&(env->lpRel), v);
  } else {
    /// bah
  }
}


// set attack lpegrator coefficient
void env_exp_set_atk_slew(env_exp* env, fract32 c) {
  filter_1p_lo_set_slew(&(env->lpAtk), c);
}


// set decay lpegrator coefficient
void env_exp_set_dec_slew(env_exp* env, fract32 c) {
  filter_1p_lo_set_slew(&(env->lpDec), c);
}


// set release lpegrator coeff
void env_exp_set_rel_slew(env_exp* env, fract32 c) {
  filter_1p_lo_set_slew(&(env->lpRel), c);
}

// set sustain duration (ignored in gated mode)
void env_exp_set_sus_dur(env_exp* env, u32 samps) {
  env->susDur = samps;
}

//-------------------------------------
//---- static function definitions

//------ next-value functions

/// attack
static fract32 next_atk(env_exp* env) {
  fract32 val;
  val = filter_1p_lo_next( &(env->lpAtk) );
  //  if( (env->lpAtk).sync) { 
  if( filter_1p_sync( &(env->lpAtk) ) ) {
    state_dec(env);
  }
  return val;
}

/// decay
static fract32 next_dec(env_exp* env) {
  fract32 val;
  val = filter_1p_lo_next( &(env->lpDec) );
  //  if( (env->lpDec).sync) { 
  if( filter_1p_sync( &(env->lpDec) ) ) {
    state_sus(env);
  }
  return val;
}

// sustain (trig mode off)
static fract32 next_sus_trig0(env_exp* env) {
  return env->valSus;
}

// sustain (trig mode on)
static fract32 next_sus_trig1(env_exp* env) {
  env->susCount += 1;
  if(env->susCount > env->susDur) {
    env->gate = 0;
    env->susCount = 0;
    state_rel(env);
  }
  return env->valSus;
}

// release
static fract32 next_rel(env_exp* env) {
  fract32 val;
  val = filter_1p_lo_next( &(env->lpRel) );
  //  if( (env->lpRel).sync) { 
  if( filter_1p_sync( &(env->lpRel) ) ) {
    state_off(env);
  }
  return val;
}

// off
static fract32 next_off(env_exp* env) {
  return 0;
}

//-----------------------------
//---- state-change functions

// attack (gate mode)
static void state_atk(env_exp* env) {
  switch(env->state) {
  case envStateOff : // expected
    // hard-reset attack integrator to off value
    env->lpAtk.y = env->valOff;
    //    env->lpAtk.sync = 0;
    filter_1p_lo_in(&(env->lpAtk), env->valOn);
    break;
  case envStateDec : 
    // hard-reset attack integrator to decay integrator value
    env->lpAtk.y = env->lpDec.y;
    //    env->lpAtk.sync = 0;
    filter_1p_lo_in(&(env->lpAtk), env->valOn);    
    break;
  case envStateRel : 
    // hard-reset attack integrator to release integrator value
    env->lpAtk.y = env->lpRel.y;
    //    env->lpAtk.sync = 0;
    filter_1p_lo_in(&(env->lpAtk), env->valOn);    
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

// attack (trigger mode)
static void state_atk_reset(env_exp* env) {
  // reset to off value
  env->lpAtk.y = env->valOff;
  //  env->lpAtk.sync = 0;
  filter_1p_lo_in(&(env->lpAtk), env->valOn);
  env->stateFP = &next_atk;
  env->state = envStateAtk;
}


// decay
static void state_dec(env_exp* env) {
  // can only enter from attack state
  env->lpDec.y = env->valOn;
  //  env->lpDec.sync = 0;
  filter_1p_lo_in(&(env->lpDec), env->valSus);
  env->stateFP = &next_dec;
  env->state = envStateDec;
}


static void state_sus(env_exp* env) {
  // can only enter from decay state
  env->susCount = 0;
  if(env->trig) {
    env->stateFP = &next_sus_trig1;
  } else {
    env->stateFP = &next_sus_trig0;
  }
  env->state = envStateSus;
}


// release
static void state_rel(env_exp* env) {
  switch(env->state) {
  case envStateSus : // expected
    // hard-reset release integrator to sustain value
    env->lpRel.y = env->valSus;
    //    env->lpRel.sync = 0;
    filter_1p_lo_in(&(env->lpRel), env->valOff);    
    break;
  case envStateAtk : 
    // hard-reset release integrator to attack integrator value
    env->lpRel.y = env->lpAtk.y;
    //    env->lpRel.sync = 0;
    filter_1p_lo_in(&(env->lpRel), env->valOff);    
    break;
  case envStateDec : 
    // hard-reset release integrator to decay integrator value
    env->lpRel.y = env->lpDec.y;
    //    env->lpRel.sync = 0;
    filter_1p_lo_in(&(env->lpRel), env->valOff);    
    break;
  case envStateOff : 
    ;;
    break;
  case envStateRel : 
    ;;
    break;
  default:
    break;
  }
  env->stateFP = &next_rel;
  env->state = envStateRel;
}

static void state_off(env_exp* env) {
  env->stateFP = &next_off;
  env->state = envStateOff;
}
