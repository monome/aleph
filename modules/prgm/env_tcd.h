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
#include "env_tcd_buffer.h"
#include "bfin_core.h"

#define FR32_MAX 0x7fffffff
#define FR32_MIN 0x80000000
#define ON 1
#define OFF 0

//number of curves
#define N_CURVES 9                  //number of curve algorithms

typedef struct _env_tcd {
    fract32 val;                    //interpolated value d[i] <-> d[i+1]

    bufferHead head;                //play&record head for sample based curves
    
    u32 countTime;                  //length counter
    u32 time;                       //length of curve
//    u32 *time;                       //length of curve
    u32 pos;                        //position
    
    fract32 (*curve)(void *);       //pointer to curve algorithm

    fract32 d;                      //curve destination
    
    u8 trig;                        //sync trig
    u8 state;                       //state ON/OFF
} env_tcd;

//initialize at pre-allocated memory
extern void env_tcd_init(env_tcd *env);

//set trig
extern void env_tcd_set_trig(env_tcd *env, u8 trig);

//set curve type
extern void env_tcd_set_curve(env_tcd *env, u8 curve);

//set curve time in samples
extern void env_tcd_set_time(env_tcd *env, u32 time);

//set level in unit x destination value
extern void env_tcd_set_dest(env_tcd *env, fract32 dest);

//set position in samples
extern void env_tcd_set_pos(env_tcd *env, u32 pos);

//set frequency in unit x destination value
//extern void env_tcd_set_dest(env_tcd *env, fract32 dest);

//set x in unit x
//extern void env_tcd_set_dest(env_tcd *env, fract32 dest);

//get next value
extern fract32 env_tcd_next(env_tcd *env);

#endif
