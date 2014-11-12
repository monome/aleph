//
//  env_tcd.c
//  aleph
//
//  Created by Staffan Jansson on 20/10/14.
//
//

#include "env_tcd.h"

//static function declarations
static fract32 (*env_tcd_get_curveptr(u8 n))();
static fract32 env_tcd_zero(env_tcd *env);
static fract32 env_tcd_bypass(env_tcd *env);
static fract32 env_tcd_gate(env_tcd *env);
static fract32 env_tcd_lin_beta(env_tcd *env);
static fract32 env_tcd_noise(env_tcd *env);
static fract32 env_tcd_wav_beta(env_tcd *env);

//extern function definitions
void env_tcd_init(env_tcd *env) {
    env->val = 0;
    env->source = 0;
    env->time = 0; //1s
    env->countTime = 0;
    env->curve = 0;
    env->dest = 0; //10v
    env->trig = 0;
    env->state = OFF;
}


//set time
void env_tcd_set_time(env_tcd *env, u32 time) {
    env->time = time;
}


//return pointer to curve algorithm
fract32 (*env_tcd_get_curveptr(u8 n))() {
    static fract32 (*curves[])() =
    {
        env_tcd_zero,
        env_tcd_bypass,
        env_tcd_gate,
        env_tcd_lin_beta,
        env_tcd_noise,
        env_tcd_wav_beta,
    };
    
    return (n < 1 || n > N_CURVES) ? *curves[0] : *curves[n];
}


//set curve
void env_tcd_set_curve(env_tcd *env, u8 n) {
    env->curve = env_tcd_get_curveptr(n);
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
void env_tcd_set_trig(env_tcd *env, u16 trig) {
    env->trig = trig;

    if(env->trig) {
        env->state = ON;
        env->countTime = 0;
        env->trig = 0;
    }
    else ;
}


//static function definitions
//curve: zero
fract32 env_tcd_zero(env_tcd *env) {
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

//curve: gate
fract32 env_tcd_gate(env_tcd *env) {
    if (env->state == ON)
    {
        if(env->countTime < env->time) { env->countTime++; return env->dest; }
        else { env->state = OFF; return env->source; }
    }
    
    else
        return env->source;
}

//curve: lin_beta
fract32 env_tcd_lin_beta(env_tcd *env) {
    if (env->source > env->dest && env->state == ON)
    {
        env->countTime++;
            
        if (env->countTime < env->time)
        {
            env->dest = sub_fr1x32(env->source, mult_fr1x32x32(0x7fff, sub_fr1x32(env->source, env->dest)));
            return env->dest;
        }
        else
        {
            env->state = OFF;
            return env->dest;
        }
    }
    else if (env->source < env->dest && env->state == ON)
    {
        env->countTime++;
            
        if (env->countTime < env->time)
        {
            env->dest = add_fr1x32(env->source, mult_fr1x32x32(0x7fff, sub_fr1x32(env->dest, env->source)));
            return env->dest;
        }
        else
        {
            env->state = OFF;
            return env->dest;
        }
    }
    else return env->dest;
}

//curve: noise
s32 env_tcd_noise(env_tcd *env) {
    if (env->state == ON)
    {
        if(env->countTime < env->time)
        {
            env->countTime++;
            //  allow overflow
            env->val = (u32)env->val * 1664525 + (u32)env->source;
            return (s32) env->val;
        }
        else
        {
            env->state = OFF;
            return 0;
        }
    }
    else
        return 0;
}

//curve: wav_beta
fract32 env_tcd_wav_beta(env_tcd *env) {
    if (env->state == ON)
    {
        if(env->countTime < env->time)
        {
            env->countTime++;
            return env->val;
        }
        else
        {
            env->state = OFF;
            return env->val;
        }
    }
    else
        return env->val;
}

/*
 UNDER CONSTRUCTION...
 
 for (env->val = 0; *env->source != NULL; env->source++) {
 env->val = *env->source + 31 * env->val;
 return env->val % FR32_MAX;

 
 //curve: noise_beta

 
 //add u8 switch
 //curve: pulse_beta
 fract32 env_tcd_pulse_beta(env_tcd *env) {
 if (env->val == 0)
 {
 if(env->countTime < env->time)
 {
 env->countTime++;
 return env->dest;
 }
 else { env->countTime = 0; env->val = 1; return env->dest; }
 }
 else if (env->val == 1)
 {
 if(env->countTime < env->time)
 {
 env->countTime++;
 return env->source;
 }
 else { env->countTime = 0; env->val = 0; return env->source; }
 }
 else return env->source;
 }

 
if (accum->wrap) { // wrapping...
    // if value needs wrapping, output the applied difference
    while (accum->val > accum->max) {
        dif = op_sub(accum->min, accum->max) - 1;
        wrap = op_add(wrap, dif);
        accum->val = op_add(accum->val, dif);
    }
    while (accum->val < accum->min) {
        dif = op_sub(accum->max, accum->min) + 1;
        wrap = op_add(wrap, dif);
        accum->val = op_add(accum->val, dif);
    }
    
    
    
    fract32 env_tcd_ramp_beta(env_tcd *env) {
        if (env->state == ON)
        {
            env->countTime++;
            if (env->source > env->dest) {
                env->val = sub_fr1x32(env->source, env->dest);
                return env->val;
            }
            if (env->source < env->dest) {
                env->val = sub_fr1x32(env->dest, env->source);
                return env->val;
            }
        }
        else
            
            
            } else { // saturating...
                if (accum->val > accum->max) {
                    wrap = dif = op_sub(accum->val, accum->max);
                    accum->val = accum->max;
                }
                if (accum->val < accum->min) {
                    wrap = dif = op_sub(accum->val , accum->min);
                    accum->val = accum->min;
                }
            }
    
    
    
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
*/