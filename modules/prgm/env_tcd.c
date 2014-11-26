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
static fract32 env_tcd_hold(env_tcd *env);
static fract32 env_tcd_trig(env_tcd *env);
static fract32 env_tcd_gate(env_tcd *env);
static fract32 env_tcd_one_beta(env_tcd *env);
static fract32 env_tcd_loop_beta(env_tcd *env);
static fract32 env_tcd_noise_beta(env_tcd *env);
static fract32 env_tcd_rec0_beta(env_tcd *env);
static fract32 env_tcd_trigrec1_beta(env_tcd *env);


//extern function definitions
void env_tcd_init(env_tcd *env) {
    env->val = 0;
    env->time = 0;
    env->pos = 0;
    env->countTime = 0;
    env->curve = env_tcd_get_curveptr(0);
    env->d = 0;
    env->trig = 0;
    env->state = OFF;
}


//set time
void env_tcd_set_time(env_tcd *env, u32 time) {
    if (env->pos - time < 0) time = env->pos;
    env->time = time;
}


//set pos
void env_tcd_set_pos(env_tcd *env, u32 pos) {
    if (pos > env->time) pos = env->time;
    env->pos = pos;
}


static fract32 (*curves[])() =
{
    env_tcd_off,            //0
    env_tcd_hold,           //1
    env_tcd_trig,           //2
    env_tcd_gate,           //3
    env_tcd_one_beta,       //4
    env_tcd_loop_beta,      //5
    env_tcd_noise_beta,     //6
    env_tcd_rec0_beta,      //7
    env_tcd_trigrec1_beta   //8
};


//return pointer to curve algorithm
fract32 (*env_tcd_get_curveptr(u8 n))() {
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
    env->d = dest;
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

//curve: hold
fract32 env_tcd_hold(env_tcd *env) {
    return env->d;
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
        env->countTime++;
        
        if(env->countTime < env->time) return 0x3fffffff;
        
        else
        {
            env->state = OFF;
            return 0;
        }
    }
    
    else
        return 0;
}

//curve: one_beta
fract32 env_tcd_one_beta(env_tcd *env) {
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        buffer_head_pos(&(env->head), env->pos);
        return buffer_head_play(&(env->head));
    }
    else if (env->state == ON && env->countTime < (env->time - env->pos))
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

//curve: loop_beta
fract32 env_tcd_loop_beta(env_tcd *env) {
    if (env->state == ON)
    {
        if (!env->countTime)
        {
            env->countTime++;
            buffer_head_pos(&(env->head), env->pos);
            return buffer_head_play(&(env->head));
        }
        else if (env->countTime < (env->time - env->pos))
        {
            env->countTime++;
            buffer_head_next(&(env->head));
            return buffer_head_play(&(env->head));
        }
        else
        {
            env->countTime = 0;
            env->countTime++;
            buffer_head_pos(&(env->head), env->pos);
            return buffer_head_play(&(env->head));
        }
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
            env->val = (u32)env->val * 1664525 + 1013904223;
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

//curve: audio_rec_beta
fract32 env_tcd_rec0_beta(env_tcd *env) {
    fract32 tmp;
    if (env->state == ON && (!env->countTime))
    {
        env->head.loop = env->time;
        env->countTime++;
        tmp = 0;
        tmp = mult_fr1x32x32(in[0], FR32_MAX >> 2);
        buffer_head_pos(&(env->head), 0);
        buffer_head_rec(&(env->head), tmp);
        return tmp;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        tmp = 0;
        tmp = mult_fr1x32x32(in[0], FR32_MAX >> 2);
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
fract32 env_tcd_trigrec1_beta(env_tcd *env) {
    if (env->state == ON && (!env->countTime))
    {
        env->head.loop = env->time;
        env->countTime++;
        fract32 tmp = add_fr1x32(tmp, mult_fr1x32x32(in[1], (FRACT32_MAX >> 10)));
        buffer_head_pos(&(env->head), 0);
        buffer_head_rec(&(env->head), tmp);
        return 0x3fffffff;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        fract32 tmp = add_fr1x32(tmp, mult_fr1x32x32(in[1], (FRACT32_MAX >> 10)));
        buffer_head_next(&(env->head));
        buffer_head_rec(&(env->head), tmp);
        if (env->countTime < 0x0000003f) return 0x3fffffff;
        else return 0;
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}
