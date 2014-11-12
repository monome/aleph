//
//  env_tcd.h
//  aleph
//
//  Created by Staffan Jansson on 20/10/14.
//
//

#ifndef aleph_env_tcd_h
#define aleph_env_tcd_h

#include "types.h"
#include "fract_math.h"

#define FR32_MAX 0x7fffffff
#define FR32_MIN 0x80000000

//trig
#define ON 1
#define OFF 0
u8 state_t;                         //trig state

#define N_CURVES 6                  //number of curve algorithms

typedef struct _env_tcd {
    fract32 val;                    //interpolated value
    
    fract32 source;                 //level at start of curve
    
    u32 time;                       //length of curve (in samples)
    u32 countTime;                  //length counter
    
    fract32 (*curve)(void *);       //pointer to curve algorithm

    fract32 dest;                   //level at end of curve

    u8 trig;                        //sync trig
    u8 state;                       //state ON/OFF
} env_tcd;

//initialize at pre-allocated memory
extern void env_tcd_init(env_tcd *env);

//set curve time in samples
extern void env_tcd_set_time(env_tcd *env, u32 time);

//set curve type
extern void env_tcd_set_curve(env_tcd *env, u8 curve);

//set destination value
extern void env_tcd_set_dest(env_tcd *env, fract32 dest);

//set trig
extern void env_tcd_set_trig(env_tcd *env, u16 trig);

//get next value
extern fract32 env_tcd_next(env_tcd *env);

#endif
