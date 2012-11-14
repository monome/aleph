/* env.c
 * aleph/null
 *
 * envelopes:
 * - ASR
 * - trapezoid ?
 *
 */

// test
#include <stdio.h>

#include <math.h>
#include "env.h" 

// max/min values of fract32
#define FR32_MAX           0x7fffffff
#define FR32_MIN           0x80000000

#define ENV_MIN            (0x800) //  -120.41199826929 db as fract32
#define ENV_MIN_DOUBLE     (9.53674316e-07)
#define ENV_MAX            (FR32_MAX)
#define ENV_MAX_DOUBLE     (1.0)

// envelope states
#define ENV_STATE_ATK_POS  0
#define ENV_STATE_ATK_NEG  1
#define ENV_STATE_REL_POS  2
#define ENV_STATE_REL_NEG  3
#define ENV_STATE_SUS      4
#define ENV_STATE_OFF      5
#define ENV_NUM_STATES     6

/*
    switch(env->state) {
    case ENV_STATE_OFF:
      break;	
    case ENV_STATE_ATK_POS  :
	break;
    case ENV_STATE_ATK_NEG  :
      break;
    case ENV_STATE_REL_POS  :
      break;
    case ENV_STATE_REL_NEG  :
      break;
    case ENV_STATE_SUS      :
      break;
      break;
*/


//-------------------------------------
//----- static function declarations

///// state processing

// start attack section
static void env_asr_attack(env_asr* env);
// start sustain section
static void env_asr_sustain(env_asr* env);
// start release section
static void env_asr_release(env_asr* env);
// turn off
static void env_asr_off(env_asr* env);

// attack stage, positive curve
static fract32 env_next_atk_pos(env_asr* env);
// attack stage, negative curve
static fract32 env_next_atk_neg(env_asr* env);
// release stage, positive curve
static fract32 env_next_rel_pos(env_asr* env);
// release stage, negative curve
static fract32 env_next_rel_neg(env_asr* env);
// sustain stage
static fract32 env_next_sus(env_asr* env);
// not playing
static fract32 env_next_off(env_asr* env);

/// set stage
static void env_set_state(env_asr* env, u8 state);

//---------------------------------------------
//---- external function defs

// intialize pre-allocated memory
void env_asr_init(env_asr* env) {
  env_asr_off(env);
}


// get the next value
fract32 env_asr_next(env_asr* env) {
  return (*(env->stateFP))(env);
}

// set attack duration in samples
void env_asr_set_atk_dur(env_asr* env, u32 dur) { 
  env->bUp = float_to_fr32( (float)( pow(ENV_MIN_DOUBLE, 1.0 / (double)dur) ) );
  env->rUp = FR32_MAX / dur;
  printf("\n env ramp increment : %d", env->rUp);
  //  env->aUp = 1.0 / env->bUp;
}

// set release duration in samples
void env_asr_set_rel_dur(env_asr* env, u32 dur) {
  env->bDn = float_to_fr32( (float)( pow(ENV_MIN_DOUBLE, 1.0 / (double)dur) ) );
  env->rDn = FR32_MAX / dur;
  //  env->aDn = 1.0 / env->bUp;
}

// set attack curve in [-1, 1]
void env_asr_set_atk_shape(env_asr* env, fract32 c) {
  env->cUp = c;
}

// set release curve in [-1, 1]
void env_asr_set_rel_shape(env_asr* env, fract32 c) {
  env->cDn = c;
}

// set gate
void env_asr_set_gate(env_asr* env, u8 g) {
  if(g > 0) {
    env_asr_attack(env);
  } else {
    switch(env->state) {
    case ENV_STATE_ATK_POS  :
    case ENV_STATE_ATK_NEG  :           
    case ENV_STATE_SUS      :
      env_asr_release(env);
      break;
    case ENV_STATE_REL_POS  :
    case ENV_STATE_REL_NEG  :
    case ENV_STATE_OFF      :
    default: // nothing
      break;
    }
  }
}


//----------------------------------------
// ---- static function defs

// perform attack section 
void env_asr_attack(env_asr* env) {
  if(env->cUp > 0) {
    // inverted decay
    env->state = ENV_STATE_ATK_POS;
    env->stateFP = *(env_next_atk_pos);
    env->x = ENV_MAX;
    env->y = 0; // ramp
  } else {
    // uninverted growth
    env->state = ENV_STATE_ATK_NEG;
    env->stateFP = *(env_next_atk_neg);
    env->x = ENV_MIN;
    env->y = 0; // ramp
  }
}

// perform sustain section
static void env_asr_sustain(env_asr* env) {
    env->state = ENV_STATE_SUS;
    env->stateFP = *(env_next_sus);
}

// perform release section
static void env_asr_release(env_asr* env) {
 if(env->cDn >= 0) {
    // uninverted decay
    env->state = ENV_STATE_REL_POS;
    env->stateFP = *(env_next_rel_pos);
    env->x = ENV_MAX;
    env->y = FR32_MAX;
  } else {
    // inverted growth
    env->state = ENV_STATE_REL_NEG;
    env->stateFP = *(env_next_rel_neg);
    env->x = ENV_MIN;
    env->y = FR32_MAX;
 }
}

// turn off
static void env_asr_off(env_asr* env) {
    env->state = ENV_STATE_OFF;
    env->stateFP = *(env_next_off);
    env->x = ENV_MIN;
}

////////////////////////
///////  state processing functions

// attack stage, positive curve
static fract32 env_next_atk_pos(env_asr* env) {
  // inverted decay
  if ( env->y == FR32_MAX ) {    
    env_asr_sustain(env);
  } else {
    env->x = mult_fr1x32x32(env->x, env->bUp);
    env->y = add_fr1x32(env->y, (fract32)env->rUp);
    if ( env->y < 0 ) {
      env->y = FR32_MAX;
    }
  }
  // interpolate for curve
  return add_fr1x32(
		    mult_fr1x32x32( sub_fr1x32(FR32_MAX, env->x), env->cUp ),
		    mult_fr1x32x32( env->y, sub_fr1x32( FR32_MAX, env->cUp) )
		    );
  
}

// attack stage, negative curve
static fract32 env_next_atk_neg(env_asr* env) {
  fract32 cabs = abs_fr1x32(env->cUp);
  // uninverted growth
  if ( env->y == FR32_MAX ) {    
    env_asr_sustain(env);
  } else {
  //  env->x = mult_fr1x32x32(env->x, env->aUp);
    env->x = (fract32)( fix16_mul( (fix16)(env->x >> 16), env->aUp) << 16);
    env->y = add_fr1x32(env->y, (fract32)env->rUp);
    if ( env->y < 0 ) {
      env->y = FR32_MAX;
    }
  }
  // interpolate for curve
  return add_fr1x32(
		    mult_fr1x32x32(env->x, cabs),
		    mult_fr1x32x32(env->y, sub_fr1x32( FR32_MAX, cabs ))
		    );
}



// release stage, positive curve
static fract32 env_next_rel_pos(env_asr* env) {
  if(env->y == 0) {
    env_asr_off(env);
  }
  // uninverted decay
  env->x = mult_fr1x32x32(env->x, env->bDn);
  env->y = sub_fr1x32(env->y, (fract32)env->rDn);
  if ( env->y < 0 ) {
    env->y = 0;
  }
  // interpolate for curve
  return add_fr1x32(
		    mult_fr1x32x32(env->x, env->cDn),
		    mult_fr1x32x32(env->y, sub_fr1x32( FR32_MAX, env->cDn) )
		    );
}

// release stage, negative curve
static fract32 env_next_rel_neg(env_asr* env) {
  //////////////// FIXME
  // inverted growth
  //  env->x = mult_fr1x32x32(env->x, env->aDn );
  //  env->y = add_fr1x32(env->y, (fract32)env->rDn);
  env->x = (fract32)( fix16_mul( (fix16)(env->x >> 16), env->aDn) << 16);
  // interpolate for curve
  return add_fr1x32(
		    mult_fr1x32x32(env->x, env->cDn),
		    mult_fr1x32x32(env->y, sub_fr1x32( FR32_MAX, env->cDn) )
		    );
}

// sustain stage
static fract32 env_next_sus(env_asr* env) {
  return FR32_MAX;
}

// not playing
static fract32 env_next_off(env_asr* env) {
  return 0;
}

