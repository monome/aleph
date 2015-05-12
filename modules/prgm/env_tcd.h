//  env_tcd.h, created by Staffan Jansson on 20/10/14.

#ifndef aleph_env_tcd_h
#define aleph_env_tcd_h

#include "types.h"
#include "fract_math.h"
#include "env_tcd_buffer.h"
#include "env_tcd_generator.h"
#include "bfin_core.h"

#define FR32_MAX 0x7fffffff
#define FR32_MIN 0x80000000

#define N_INPUTS 26                 //number of selectable inputs
#define N_CURVES 10                 //number of curve algorithms
#define N_HEADS 2                   //number of play|record heads

typedef struct _sample {
    s32 i;
    fract32 fr;
    
    fract32 c; // integration coefficient
    fract32 x; // target value
    fract32 y; // filtered value

} sample;

typedef struct _env_tcd {
    fract32 (*curve)(void *);       //pointer to curve algorithm

    fract32 (*input)(void *);       //pointer to input
    fract32 inL;                    //input level
    
    bufferHead head[N_HEADS];       //play|record heads
    fract32 hdF;                    //play|record feedback
    
    fract32 env;
    fract32 mu;
    fract32 decay;
    fract32 offset;
    fix32 inc;
    
    u8 trig;
    u8 state;
} env_tcd;


//initialize at pre-allocated memory
extern void env_tcd_init(env_tcd *env);

//trig curve
extern void env_tcd_set_trig(env_tcd *env);

//set curve
extern void env_tcd_set_curve(env_tcd *env, u8 curve);

//set input
extern void env_tcd_set_input(env_tcd *env, u8 n);

//set input level
extern void env_tcd_set_level(env_tcd *env, fract32 level);

//set feedback
extern void env_tcd_set_feedback(env_tcd *env, fract32 mix);

//set head offsets
extern void env_tcd_set_start(env_tcd *env, u32 offset);
extern void env_tcd_set_loop(env_tcd *env, u32 offset);

//set mu
extern void env_tcd_set_decay(env_tcd *env, s32 decay);

#endif
