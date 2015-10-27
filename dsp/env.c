/* env.c
 * aleph/null
 *
 * envelopes:
 * - ASR
 * - AHDSR ?
 * - trapezoid ?
 *
 */

///// test
//#include <stdio.h>
/////

#ifdef ARCH_BFIN
#include "fract_math.h"
#include <fract2float_conv.h>
#else
#include "fract32_emu.h"
#endif

#include <math.h>
#include "env.h" 

// max/min values of fract32
#define FR32_MAX      0x7fffffff
#define FR32_MIN      0x80000000
// max/min of env in fract32 and double
#define ENV_MIN       0x7ff //  -120.41624045437 db
#define ENV_MIN_D     9.5320865556281e-07
#define ENV_MAX       FR32_MAX
#define ENV_MAX_D     0.9999999995343386

// envelope states
#define ENV_STATE_ATK      0
#define ENV_STATE_REL      1
#define ENV_STATE_SUS      2
#define ENV_STATE_OFF      3
#define ENV_NUM_STATES     4

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
//static void env_set_state(env_asr* env, u8 state);

//---------------------------------------------
//---- external function defs

// intialize pre-allocated memory
void env_asr_init(env_asr* env) {
  env_asr_off(env);
}

// set attack duration in samples
void env_asr_set_atk_dur(env_asr* env, u32 dur) { 
  f32 fB = powf(ENV_MIN_D, 1.0 / (f32)dur);
  env->bUp = float_to_fr32(fB);
  env->aUp = float_to_fr32(1.f / fB);
  env->rUp = FR32_MAX / dur;
  //  printf("\n env growth coefficient : %08x", env->aUp);
  //  printf("\n env decay coefficient : %08x", env->bUp);
  //  printf("\n env ramp increment : %08x", env->rUp);
  //  env->aUp = 1.0 / env->bUp;
}

// set release duration in samples
void env_asr_set_rel_dur(env_asr* env, u32 dur) {
  env->bDn = float_to_fr32( (float)( pow(ENV_MIN_D, 1.0 / (double)dur) ) );
  env->rDn = FR32_MAX / dur;
  //  env->aDn = 1.0 / env->bUp;
}

// set attack curve in [-1, 1]
void env_asr_set_atk_shape(env_asr* env, fract32 c) {
  fract32 cOld = env->cUp;
  env->cUp = c;
  if((cOld < 0) != (c < 0)) {
    if (c < 0) {
      env->stateFP = &(env_next_atk_neg);
    } else {
      env->stateFP = &(env_next_atk_pos);
    }
    if(env->state == ENV_STATE_REL) {
      // invert current phase
      env->y = sub_fr1x32(FR32_MAX, env->y);
      env->x = sub_fr1x32(FR32_MAX, env->x);
    }
  }
}

// set release curve in [-1, 1]
void env_asr_set_rel_shape(env_asr* env, fract32 c) {
  fract32 cOld = env->cDn;
  env->cDn = c;
  if((cOld < 0) != (c < 0)) {
    if (c < 0) {
      env->stateFP = &(env_next_rel_neg);
    } else {
      env->stateFP = &(env_next_rel_pos);
    }
    if(env->state == ENV_STATE_REL) {
    // invert current phase
      env->y = sub_fr1x32(FR32_MAX, env->y);
      env->x = sub_fr1x32(FR32_MAX, env->x);
    }
  }
}

// set gate
void env_asr_set_gate(env_asr* env, u8 g) {
  //printf("\rsetting asr gate: %d\n", g);
  if(g > 0) {
    env_asr_attack(env);
  } else {
    if( (env->state == ENV_STATE_ATK) || (env->state == ENV_STATE_SUS) ) {
      env_asr_release(env);
    }
    /*    
    switch(env->state) {
    case ENV_STATE_ATK  :
    case ENV_STATE_SUS      : 
      env_asr_release(env);
      break;
    case ENV_STATE_REL  :
    case ENV_STATE_OFF      :
    default:
      break;
    }
    */
  }
}

// get the next value
fract32 env_asr_next(env_asr* env) {
  return (*(env->stateFP))(env);
}
 

//----------------------------------------
// ---- static function defs

// perform attack section 
void env_asr_attack(env_asr* env) {
  if(env->cUp > 0) { // positive attack curve
    switch(env->state) {
    case ENV_STATE_ATK:
    case ENV_STATE_SUS:
      // nothing to do
      break;
    case ENV_STATE_OFF:
      env->y = ENV_MAX;
      env->x = 0;
      env->stateFP = &(env_next_atk_pos);
      env->state = ENV_STATE_ATK;
      break;
    case ENV_STATE_REL:
      if(env->cDn >= 0) {
	// release is uninverted decay, want inverted decay
	env->y = sub_fr1x32(FR32_MAX, env->y);
      }
      env->stateFP = &(env_next_atk_pos);
      env->state = ENV_STATE_ATK;
      break;
    default:
      break;
    }
  } else { // negative attack curve
    switch(env->state) {
    case ENV_STATE_ATK:
    case ENV_STATE_SUS:
      // nothing to do
      break;
    case ENV_STATE_OFF:
      env->y = ENV_MAX;
      env->x = 0;
      env->stateFP = &(env_next_atk_neg);
      env->state = ENV_STATE_ATK;
      break;
    case ENV_STATE_REL:
      if(env->cDn < 0) {
	// release is inverted growth, want uninverted growth
	env->y = sub_fr1x32(FR32_MAX, env->y);
      }
      /// WTF how is this so wrong
      env->stateFP = &(env_next_atk_neg);
      env->state = ENV_STATE_ATK;
      break;
      
    default:
      break;
    }
  }
}

// perform sustain section
static void env_asr_sustain(env_asr* env) {
    env->state = ENV_STATE_SUS;
    env->stateFP = &(env_next_sus);
}

// perform release section
/// FIXME: release during attack
static void env_asr_release(env_asr* env) {
    env->state = ENV_STATE_REL;
 if(env->cDn >= 0) {
    // uninverted decay
    env->stateFP = &(env_next_rel_pos);
    env->y = ENV_MAX;
    env->x = FR32_MAX;
  } else {
    // inverted growth
    env->stateFP = &(env_next_rel_neg);
    env->y = ENV_MIN;
    env->x = FR32_MAX;
 }
}

// turn off (immediately)
static void env_asr_off(env_asr* env) {
    env->state = ENV_STATE_OFF;
    env->stateFP = &(env_next_off);
    env->y = ENV_MIN;
}

////////////////////////
///////  state processing functions

// attack stage, positive curve
static fract32 env_next_atk_pos(env_asr* env) {
  // inverted decay
  if ( env->x == FR32_MAX ) {    
    env_asr_sustain(env);
  } else {
    env->y = mult_fr1x32x32(env->y, env->bUp);
    env->x = add_fr1x32(env->x, (fract32)env->rUp);
    if ( env->x < 0 ) {
      env->x = FR32_MAX;
    }
  }
  // interpolate for curve
  return add_fr1x32( // invert x:
		    mult_fr1x32x32( sub_fr1x32(FR32_MAX, env->y), env->cUp ),
		    mult_fr1x32x32( env->x, sub_fr1x32( FR32_MAX, env->cUp) )
		    );
  
}

// attack stage, negative curve
static fract32 env_next_atk_neg(env_asr* env) {
  fract32 cabs = abs_fr1x32(env->cUp);
  // uninverted growth
  if ( env->x == FR32_MAX ) {    
    env_asr_sustain(env);
  } else {
  //  env->y = mult_fr1x32x32(env->y, env->aUp);
    env->y = (fract32)( fix16_mul( (fix16)(env->y >> 16), env->aUp) << 16);
    env->x = add_fr1x32(env->x, (fract32)env->rUp);
    if ( env->x < 0 ) {
      env->x = FR32_MAX;
    }
  }
  // interpolate for curve
  return add_fr1x32(
		    mult_fr1x32x32(env->y, cabs),
		    mult_fr1x32x32(env->x, sub_fr1x32( FR32_MAX, cabs ))
		    );
}



// release stage, positive curve
static fract32 env_next_rel_pos(env_asr* env) {
  if(env->x == 0) {
    env_asr_off(env);
  }
  // uninverted decay
  env->y = mult_fr1x32x32(env->y, env->bDn);
  env->x = sub_fr1x32(env->x, (fract32)env->rDn);
  if ( env->x < 0 ) {
    env->x = 0;
  }
  // interpolate for curve
  return add_fr1x32(
		    mult_fr1x32x32( env->y, env->cDn ),
		    mult_fr1x32x32( env->x, sub_fr1x32(FR32_MAX, env->cDn) )
		    );
}

// release stage, negative curve
static fract32 env_next_rel_neg(env_asr* env) {
  // inverted growth
  env->x = sub_fr1x32(env->x, (fract32)env->rDn);
  //  env->y = mult_fr1x32x32(env->y, env->aDn );
  env->y = (fract32)( fix16_mul( (fix16)(env->y >> 16), env->aDn) << 16);
  // interpolate for curve
  return add_fr1x32( // invert x:
		    mult_fr1x32x32( sub_fr1x32(FR32_MAX, env->y), env->cDn ),
		    mult_fr1x32x32( env->x, sub_fr1x32(FR32_MAX, env->cDn) )
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
