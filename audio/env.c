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

///// fit when switching mid-segment
// pos atk -> pos rel
static void env_asr_fit_papr(env_asr* env) {
  // pos atk: x is increasing, y is inverted/decreasing
  // pos rel: x is decreasing, y is decreasing
  float fy1;
  float fx0;
  float fc0;
  float fc1;
  // invert y
  env->y = sub_fr1x32(FR32_MAX, env->y);
  // if up and down curves are equal, we're done
  if(env->cUp == env->cDn) {
    return;
  }
  // otherwise, need to fudge x so the output value doesn't change.
  // previous output equation:
  // (1-y0)*c0 + x0*(1-c0)
  // new output equation:
  // y1*c1+ x1*(1-c1), where y1 = (1 = y0)
  // solving for x1:
  // x1 = ( y1*(c0 - c1) + x0*(1 - c0) ) / (1 - c1)
  fy1 = fr32_to_float(env->y);
  fx0 = fr32_to_float(env->x);
  fc0 = fr32_to_float(env->cUp);
  fc1 = fr32_to_float(env->cDn);
  // FIXME: can maybe refactor more efficiently ?
  env->x = float_to_fr32( ( fy1*(fc0 - fc1) + fx0*(1.f - fc0) ) / (1.f - fc1) );
}

// pos atk -> neg rel
static void env_asr_fit_panr(env_asr* env) {
  // pos atk: x is increasing, y is inverted / decreasing
  // neg rel: x is decreasing, y is inverted / increasing
  float fy1;
  float fx0;
  float fc0;
  float fc1;
  // if up and down curves are equal, we're done
  /// FIXME: can restructure to skip the prolog in this case
  if(env->cUp == env->cDn) {
    return;
  }
  // otherwise, we need to fudge x so the output value doesn't change.
  // both output equations:
  // (1-y)*c + x*(1-c)
  // c has changed, solving for x:
  // x1 = ( c0(1 - y - x0) + x0 + c1(y - 1)) / (1 - c1)
  fy1 = fr32_to_float(env->y);
  fx0 = fr32_to_float(env->x);
  fc0 = fr32_to_float(env->cUp);
  fc1 = fr32_to_float(env->cDn);
  env->x = float_to_fr32( ( fc0*(1.f - fy1 - fx0) + fx0 + fc1*(fy1 - 1.f)) / (1.f - fc1) );
}

// neg atk -> pos rel
static void env_asr_fit_napr(env_asr* env) {
  // TODO: not using negative curves just yet
}

// neg atk -> neg rel
static void env_asr_fit_nanr(env_asr* env) {
  // TODO: not using negative curves just yet
}

// pos rel -> pos atk
static void env_asr_fit_prpa(env_asr* env) {
  // pos rel: x decreasing, y decreasing
  // pos atl: x increasing, y inverted / decreasing
  float fy0;
  float fx0;
  float fc0;
  float fc1;
  fy0 = fr32_to_float(env->y);
  /// invert integrator
  env->y = sub_fr1x32(FR32_MAX, env->y);
  /// if curves are equal, we're done
  if(env->cUp == env->cDn) {
    return;
  }
  // otherwise, need to fudge x to make output equal
  // old output = (c0 * y0) + (x0 * (1 - c0))
  // new output = (c1 * (1-y1)) + (x1 * (1-c1))
  // solving for x1:
  // x1 = ( y0*(c0 - c1) + x0 - x0*c0 ) / (1 - c1)
  fx0 = fr32_to_float(env->x);
  fc0 = fr32_to_float(env->cUp);
  fc1 = fr32_to_float(env->cDn);
  env->x = float_to_fr32( ( fy0 * (fc0 - fc1) + fx0 - (fx0 * fc0) ) / (1.f - fc1) );
}

// pos rel -> neg atk
static void env_asr_fit_prna(env_asr* env) {
  // TODO: not using negative curves just yet
  ;;
}
// neg rel -> pos atk
static void env_asr_fit_nrpa(env_asr* env) {
  // TODO: not using negative curves just yet
  ;;
}

// neg rel -> neg atk
static void env_asr_fit_nrna(env_asr* env) {
  // TODO: not using negative curves just yet
  ;;
}


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
}

// set release duration in samples
void env_asr_set_rel_dur(env_asr* env, u32 dur) {
  env->bDn = float_to_fr32( (float)( pow(ENV_MIN_D, 1.0 / (double)dur) ) );
  env->rDn = FR32_MAX / dur;
}

// set attack curve in [-1, 1]
void env_asr_set_atk_shape(env_asr* env, fract32 c) {
  fract32 cOld = env->cUp;
  env->cUp = c;
  if((cOld < 0) != (c < 0)) {
    if (c < 0) {
      env->stateFP = *(env_next_atk_neg);
    } else {
      env->stateFP = *(env_next_atk_pos);
    }
    if(env->state == ENV_STATE_REL) {
      // invert current phase
      ///// FIXME: among other things related to negative curves, this seems wrong.
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
      env->stateFP = *(env_next_rel_neg);
    } else {
      env->stateFP = *(env_next_rel_pos);
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
      // start atk segment from beginning
      // inverted decay, so integrator starts at 1
      env->y = ENV_MAX;
      env->x = 0;
      env->stateFP = *(env_next_atk_pos);
      env->state = ENV_STATE_ATK;
      break;
    case ENV_STATE_REL:
      if(env->cDn >= 0) {
	env_asr_fit_prpa(env);
      } else {
	env_asr_fit_nrpa(env);
      }
      env->stateFP = *(env_next_atk_pos);
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
      // uninverted growth, integrator starts at min
      env->y = ENV_MIN;
      env->x = 0;
      env->stateFP = *(env_next_atk_neg);
      env->state = ENV_STATE_ATK;
      break;
    case ENV_STATE_REL:
      if(env->cDn >= 0) {
	env_asr_fit_prna(env);
      } else {
	env_asr_fit_nrna(env);
      }
      env->stateFP = *(env_next_atk_neg);
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
    env->stateFP = *(env_next_sus);
}

// perform release section
static void env_asr_release(env_asr* env) {
  if(env->state == ENV_STATE_ATK) {
    if(env->cDn >= 0) {
      if(env->cUp >= 0) {
	env_asr_fit_papr(env);
      } else {
	env_asr_fit_napr(env);
      }
      // uninverted decay
      env->stateFP = *(env_next_rel_pos);
    } else {
      if(env->cUp >= 0) {
	env_asr_fit_panr(env);
      } else {
	env_asr_fit_nanr(env);
      }
      env->stateFP = *(env_next_rel_pos);      
    }
  } else {
    if(env->cDn >= 0) {
      // uninverted decay
      env->stateFP = *(env_next_rel_pos);
      env->y = ENV_MAX;
      env->x = FR32_MAX;
    } else {
      // inverted growth
      env->stateFP = *(env_next_rel_neg);
      env->y = ENV_MIN;
      env->x = FR32_MAX;
    }
  }
  env->state = ENV_STATE_REL;
}

// turn off (immediately)
static void env_asr_off(env_asr* env) {
    env->state = ENV_STATE_OFF;
    env->stateFP = *(env_next_off);
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

  env->out =  add_fr1x32( // invert y
		    mult_fr1x32x32( sub_fr1x32(FR32_MAX, env->y), env->cUp ),
		    mult_fr1x32x32( env->x, sub_fr1x32( FR32_MAX, env->cUp) )
		    );
  return env->out;
  
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
    if ( env->x < 0 ) { // overflow
      env->x = FR32_MAX;
    }
  }
  // interpolate for curve
  env->out = add_fr1x32(
		    mult_fr1x32x32(env->y, cabs),
		    mult_fr1x32x32(env->x, sub_fr1x32( FR32_MAX, cabs ))
		    );
  return env->out;
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
  env->out = add_fr1x32(
		    mult_fr1x32x32( env->y, env->cDn ),
		    mult_fr1x32x32( env->x, sub_fr1x32(FR32_MAX, env->cDn) )
		    );
  return env->out;
}

// release stage, negative curve
static fract32 env_next_rel_neg(env_asr* env) {
  // inverted growth
  env->x = sub_fr1x32(env->x, (fract32)env->rDn);
  //  env->y = mult_fr1x32x32(env->y, env->aDn );
  env->y = (fract32)( fix16_mul( (fix16)(env->y >> 16), env->aDn) << 16);
  // interpolate for curve
  env->out = add_fr1x32( // invert y:
		    mult_fr1x32x32( sub_fr1x32(FR32_MAX, env->y), env->cDn ),
		    mult_fr1x32x32( env->x, sub_fr1x32(FR32_MAX, env->cDn) )
		    );
  return env->out;
}

// sustain stage
static fract32 env_next_sus(env_asr* env) {
  return FR32_MAX;
}

// not playing
static fract32 env_next_off(env_asr* env) {
  return 0;
}


