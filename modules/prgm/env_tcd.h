//  env_tcd.h, created by Staffan Jansson on 20/10/14.

#ifndef aleph_env_tcd_h
#define aleph_env_tcd_h

#include "types.h"
#include "fract_math.h"
#include "env_tcd_buffer.h"
#include "bfin_core.h"

#define FR32_MAX 0x7fffffff         //2147483647
#define FR32_MIN 0x80000000         //2147483648
#define FR32_MAX1_2 0x3fffffff
#define FR32_MAX1_3 0x2aaaaaaa
#define FR32_MAX1_4 0x1fffffff
#define FR32_MAX1_6 0x15555555
#define FR32_MAX1_8 0x0fffffff
#define FR32_MAX1_16 0x07ffffff
#define FR32_MAX1_24 0x05555555
#define FR32_MAX1_48 0x02aaaaaa
#define FR32_MAX2_3 0x55555555
#define FR32_MAX5_8 0x4fffffff
#define FR32_MAX8_9 0x71c71c71
#define FR32_MAX9_16 0x47ffffff
#define FR32_MAX23_48 0x3d555555

#define FR32_MAX2 0xffffffff
#define FR32_MAX3_2 0xbfffffff
#define FR32_MAX5_2 0x13fffffff
#define FR32_MAX9_8 0x8fffffff
#define FR32_MAX11_8 0xafffffff

#define N_INPUTS 21                 //number of selectable inputs
#define N_CURVES 25                 //number of curve algorithms
#define N_HEADS 2                   //number of play|record heads //REMOVE?

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
    
    fract32 decay;                  //envelope decay
    fract32 x;
    fract32 c0;
    fract32 c1;
    fract32 c2;
    fract32 c3;
    fract32 env;
    
    u8 trig;
    u8 state;
    u8 motor;
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

//set decay
extern void env_tcd_set_decay(env_tcd *env, fract32 decay);

//set motor
extern void env_tcd_set_motor(env_tcd *env, u8 state);

#endif
