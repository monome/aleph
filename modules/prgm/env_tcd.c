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
static fract32 env_tcd_off(env_tcd *env);
static fract32 env_tcd_bypass(env_tcd *env);
static fract32 env_tcd_trig(env_tcd *env);
static fract32 env_tcd_gate(env_tcd *env);
static fract32 env_tcd_play_beta(env_tcd *env);
static fract32 env_tcd_loop_beta(env_tcd *env);
static fract32 env_tcd_lin_beta(env_tcd *env);
static fract32 env_tcd_noise_beta(env_tcd *env);
static fract32 env_tcd_rec0_beta(env_tcd *env);
static fract32 env_tcd_rec1_beta(env_tcd *env);

//pending...
static fract32 env_tcd_wav_beta(env_tcd *env);

//extern function definitions
void env_tcd_init(env_tcd *env) {
    env->val = 0;
    env->source = 0;
    env->time = 0;
    env->countTime = 0;
    env->curve = 0;
    env->dest = 0;
    env->trig = 0;
    env->state = OFF;
}


//set time
void env_tcd_set_time(env_tcd *env, u32 time) {
    env->time = time;
}

//ADD #ifdefine based on flag!!!
//return pointer to curve algorithm
fract32 (*env_tcd_get_curveptr(u8 n))() {
    static fract32 (*curves[])() =
    {
        env_tcd_off,
        env_tcd_bypass,
        env_tcd_trig,
        env_tcd_gate,
        env_tcd_play_beta,
        env_tcd_loop_beta,
        env_tcd_lin_beta,
        env_tcd_noise_beta,
        env_tcd_rec0_beta,
        env_tcd_rec1_beta,
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
//curve: off
fract32 env_tcd_off(env_tcd *env) {
    if (env->state == ON) { env->state = OFF; return 0; }
    else return 0;
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

//curve: trig
fract32 env_tcd_trig(env_tcd *env) {
    if (env->state == ON)
    {
        if(env->countTime < 0x0000003f) { env->countTime++; return 0x3fffffff; }
        else { env->state = OFF; return 0; }
    }
    
    else
        return 0;
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

//curve: cv_play_beta
fract32 env_tcd_play_beta(env_tcd *env) {
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        buffer_head_pos(&(env->head), 0);
        return buffer_head_play(&(env->head));
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        buffer_head_next(&(env->head));
        return buffer_head_play(&(env->head));
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: cv_loop_beta
fract32 env_tcd_loop_beta(env_tcd *env) {
    if (env->state == ON)
    {
        if (!env->countTime)
        {
            env->countTime++;
            buffer_head_pos(&(env->head), 0);
            return buffer_head_play(&(env->head));
        }
        else if (env->countTime < env->time)
        {
            env->countTime++;
            buffer_head_next(&(env->head));
            return buffer_head_play(&(env->head));
        }
        else
        {
            env->countTime = 0;
            env->countTime++;
            buffer_head_pos(&(env->head), 0);
            return buffer_head_play(&(env->head));
        }
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: audio_rec_beta
fract32 env_tcd_rec0_beta(env_tcd *env) {
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        fract32 tmp = add_fr1x32(tmp, mult_fr1x32x32(in[0], (FRACT32_MAX >> 10)));
        buffer_head_pos(&(env->head), 0);
        buffer_head_rec(&(env->head), tmp);
        return tmp;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        fract32 tmp = add_fr1x32(tmp, mult_fr1x32x32(in[0], (FRACT32_MAX >> 10)));
        buffer_head_next(&(env->head));
        buffer_head_rec(&(env->head), tmp);
        return tmp;
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: audio_rec_beta
fract32 env_tcd_rec1_beta(env_tcd *env) {
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        fract32 tmp = add_fr1x32(tmp, mult_fr1x32x32(in[1], (FRACT32_MAX >> 10)));
        buffer_head_pos(&(env->head), 0);
        buffer_head_rec(&(env->head), tmp);
        return tmp;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        fract32 tmp = add_fr1x32(tmp, mult_fr1x32x32(in[1], (FRACT32_MAX >> 10)));
        buffer_head_next(&(env->head));
        buffer_head_rec(&(env->head), tmp);
        return tmp;
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: noise
s32 env_tcd_noise_beta(env_tcd *env) {
    if (env->state == ON)
    {
        if (env->countTime < env->time)
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
/*
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
