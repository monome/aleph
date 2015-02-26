//  env_tcd.h, created by Staffan Jansson on 20/10/14.

#ifndef aleph_env_tcd_h
#define aleph_env_tcd_h

#include "types.h"
#include "fract_math.h"
#include "env_tcd_buffer.h"
#include "env_tcd_bandlimit.h"
#include "bfin_core.h"

#define FR32_MAX 0x7fffffff
#define FR32_MIN 0x80000000

#define OFF 0
#define ON 1
#define ARM 2

//number of curves
#define N_CURVES 13                 //number of curve algorithms

//number of inputs
#define N_INPUTS 26                 //number of selectable inputs

#define N_HEADS 6                   //number of play|record heads
#define N_HEADS_1 (N_HEADS - 1)           


typedef struct _env_tcd {
    fract32 (*curve)(void *);       //pointer to curve algorithm

    fract32 (*inputA)(void *);      //pointer to input A
    fract32 (*inputB)(void *);      //pointer to input B
    
    bufferHead head[N_HEADS];       //play&record heads
//    fract32 history[N_HEADS];       //history buffer
//    fract32 mix[N_HEADS];           //mix|mix feedback
    fract32 mix;                    //mix|mix feedback

    
    u32 countTime;                  //curve length counter
    u32 time;                       //length of curve

    u8 trig;                        //curve sync trig
    u8 state;                       //curve state ON/OFF
    u8 headState;                   //recording head state OFF/ARM/ON

} env_tcd;


//initialize at pre-allocated memory
extern void env_tcd_init(env_tcd *env);

//set trig
extern void env_tcd_set_trig(env_tcd *env, u8 trig);

//set curve
extern void env_tcd_set_curve(env_tcd *env, u8 curve);

//set input A
extern void env_tcd_set_inputA(env_tcd *env, u8 n);

//set input B
extern void env_tcd_set_inputB(env_tcd *env, u8 n);

//set mix
extern void env_tcd_set_mix(env_tcd *env, fract32 val);

//set head start position
extern void env_tcd_set_offset(env_tcd *env, u32 offset);

//set head loop point
extern void env_tcd_set_loop(env_tcd *env, u32 loop);

//set curve time in samples
extern void env_tcd_set_time(env_tcd *env, u32 time);

#endif
