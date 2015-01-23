//  env_tcd.h, created by Staffan Jansson on 20/10/14.

#ifndef aleph_env_tcd_h
#define aleph_env_tcd_h

#include "types.h"
#include "fract_math.h"
#include "env_tcd_buffer.h"
#include "bfin_core.h"

#define FR32_MAX 0x7fffffff
#define FR32_MIN 0x80000000

#define OFF 0
#define ON 1
#define ARM 2

//number of curves
#define N_CURVES 9                  //number of curve algorithms

//number of inputs
#define N_INPUTS 8                  //number of selectable physical inputs

typedef struct _env_tcd {
    fract32 (*curve)(void *);       //pointer to curve algorithm
    
    fract32 *input;                 //pointer to physical inputs
    bufferHead head;                //play&record head
    u32 pos;                        //position within offset
    
//    u32 countFrame;                 //step length counter
    u32 countTime;                  //curve length counter
    u32 time;                       //length of curve
    
    u8 trig;                        //curve sync trig
    u8 state;                       //curve state ON/OFF
    u8 headState;                   //recording head state OFF/ARM/ON    

    fract32 val;                    //interpolated value source | destination
} env_tcd;

//initialize at pre-allocated memory
extern void env_tcd_init(env_tcd *env);

//set trig
extern void env_tcd_set_trig(env_tcd *env, u8 trig);

//set physical input
extern void env_tcd_set_input(env_tcd *env, u8 n);

//set curve
extern void env_tcd_set_curve(env_tcd *env, u8 curve);

//set start position
extern void env_tcd_set_pos(env_tcd *env, u32 pos);

//set loop point
extern void env_tcd_set_loop(env_tcd *env, u32 loop);

//set curve time in samples
extern void env_tcd_set_time(env_tcd *env, u32 time);

//get next value
extern fract32 env_tcd_next(env_tcd *env);

#endif
