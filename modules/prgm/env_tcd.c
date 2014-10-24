//
//  env_tcd.c
//  aleph
//
//  Created by Staffan Jansson on 20/10/14.
//
//

#include "env_tcd.h"

//static function declarations
static fract32 (*env_tcd_set_curveptr(u8 n))();
static fract32 env_tcd_off(env_tcd *env);
static fract32 env_tcd_bypass(env_tcd *env);
static fract32 env_tcd_hold(env_tcd *env);
static fract32 env_tcd_test(env_tcd *env);


//extern function definitions
void env_tcd_init(env_tcd *env) {
    env->val = 0;
    env->source = 0;
    env->time = 4800; //1s
    env->countTime = 0;
    env->curve = 0;
    env->dest = 0x7fffffff; //10v
    env->trig = 0;
    env->state = OFF;
}


//set time
void env_tcd_set_time(env_tcd *env, u32 time) {
    env->time = time;
}


//return pointer to curve algorithm
fract32 (*env_tcd_set_curveptr(u8 n))() {
    static fract32 (*curves[])() =
    {
        env_tcd_off,
        env_tcd_bypass,
        env_tcd_hold,
        env_tcd_test
    };
    
    return (n < 1 || n > N_CURVES) ? *curves[0] : *curves[n];
}


//set curve
void env_tcd_set_curve(env_tcd *env, u8 n) {
    env->curve = env_tcd_set_curveptr(n);
}


//get next value
fract32 env_tcd_next(env_tcd *env) {
    return env->curve(env);
}


//set destination
void env_tcd_set_dest(env_tcd *env, fract32 dest) {
    env->dest = dest;
}


//set trig
void env_tcd_set_trig(env_tcd *env, u8 trig) {
    env->trig = trig;
    
    if(env->trig == 0)
        state_t = OFF;
    
    else if(state_t == OFF) {
        state_t = ON;
        env->countTime = 0;
        env->state = ON;
    }
    
    else if(state_t == ON)
        ;;
}


//static function definitions
//curve: off
fract32 env_tcd_off(env_tcd *env) {
    if (env->state == ON)
    {
        env->state = OFF; return 0;
    }
    
    else
        return 0;
}

//curve: bypass
fract32 env_tcd_bypass(env_tcd *env) {
    if (env->state == ON)
    {
        env->state = OFF; return env->source;
    }
    
    else
        return env->source;
}

//curve: hold
fract32 env_tcd_hold(env_tcd *env) {
    if (env->state == ON)
    {
        if(env->countTime < env->time) { env->countTime++; return env->dest; }
        else { env->state = OFF; return env->source; }
    }
    
    else
        return env->source;
}

//curve: ramp_beta
fract32 env_tcd_test(env_tcd *env) {
    if (env->state == ON)
    {
        if(env->countTime < env->time)
        {
            env->countTime++;
            if (env->val == 0)
            {
                env->val = env->source;
                return env->val;
            }
            else if (env->val > env->dest)
            {
                env->val = sub_fr1x32(env->val, 0x7ffff);
                return env->val;
            }
            else
            {
                env->val = 0;
                env->countTime = env->time;
                return env->dest;
            }
            
        }
        else { env->state = OFF; return env->dest; }
    }
    
    else
        return env->dest;
}
