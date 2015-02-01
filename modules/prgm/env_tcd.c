//  env_tcd.c, created by Staffan Jansson on 20/10/14.

#include "env_tcd.h"

//static function declarations
static fract32 (*env_tcd_get_curveptr(u8 n))();
static fract32 (*env_tcd_get_inputptr(u8 n))();

static fract32 env_tcd_off(env_tcd *env);
static fract32 env_tcd_dummy(env_tcd *env);
static fract32 env_tcd_trig(env_tcd *env);
static fract32 env_tcd_gate(env_tcd *env);
static fract32 env_tcd_one(env_tcd *env);
static fract32 env_tcd_loop(env_tcd *env);
static fract32 env_tcd_noise(env_tcd *env);
static fract32 env_tcd_rec(env_tcd *env);
static fract32 env_tcd_trigrec(env_tcd *env);
static fract32 env_tcd_auxmix(env_tcd *env);

static fract32 env_tcd_in0(void);
static fract32 env_tcd_in1(void);
static fract32 env_tcd_in0_1(void);
static fract32 env_tcd_aux(void);
static fract32 env_tcd_out0(void);
static fract32 env_tcd_out1(void);
static fract32 env_tcd_out2(void);
static fract32 env_tcd_out3(void);


//function definitions
void env_tcd_init(env_tcd *env) {
    env->curve = env_tcd_get_curveptr(0);
    env->input = env_tcd_get_inputptr(0);

    //  env->head is handled in module_init()
    env->pos = 0;

    env->countTime = 0;
    env->time = 0;

    env->trig = 0;
    env->state = OFF;
    env->headState = OFF;

    env->val = 0;
}


//return pointer to curve algorithm
static fract32 (*env_tcd_get_curveptr(u8 n))() {
    static fract32 (*curves[])() =
    {
        env_tcd_off,            //0
        env_tcd_dummy,          //1
        env_tcd_trig,           //2
        env_tcd_gate,           //3
        env_tcd_one,            //4
        env_tcd_loop,           //5
        env_tcd_noise,          //6
        env_tcd_rec,            //7
        env_tcd_trigrec,        //8
        env_tcd_auxmix,         //9
    };

    return (n < 1 || n > N_CURVES) ? *curves[0] : *curves[n];
}


//set curve
void env_tcd_set_curve(env_tcd *env, u8 n) {
    env->curve = env_tcd_get_curveptr(n);
}


//return pointer to physical input
fract32 (*env_tcd_get_inputptr(u8 n))() {
    static fract32 (*inputs[])() =
    {
        env_tcd_in0,            //input 1
        env_tcd_in1,            //input 2
        env_tcd_in0_1,          //input 1 & 2 mix
        env_tcd_aux,            //aux mix
        env_tcd_out0,           //output 1
        env_tcd_out1,           //output 2
        env_tcd_out2,           //output 3
        env_tcd_out3,           //output 4
    };
    
    return (n < 1 || n > N_INPUTS) ? *inputs[0] : *inputs[n];
}


//set input
void env_tcd_set_input(env_tcd *env, u8 n) {
    env->input = env_tcd_get_inputptr(n);
}


//set start position (within offset)
void env_tcd_set_pos(env_tcd *env, u32 pos) {
    if (pos > env->head.loop) pos = env->head.loop;
    env->pos = pos;
}


//set loop point (= start point of the next offset)
extern void env_tcd_set_loop(env_tcd *env, u32 loop) {
    env->head.loop = loop;
}


//set time (in relation to offset and position)
void env_tcd_set_time(env_tcd *env, u32 time) {
    env->time = time;
}


//set trig
void env_tcd_set_trig(env_tcd *env, u8 trig) {
    env->trig = trig;

    if(env->trig) {
        env->state = ON;
        env->countTime = 0;
        env->trig = 0;
    }
    else ;
}


//break out frame count
//handle frame count
//u32 env_tcd_frameCount(env_tcd *env) {
//    env->countFrame++;
//return framecount
//}


//static function definitions
//curve: off
fract32 env_tcd_off(env_tcd *env) {
    if (env->state == ON) { env->state = OFF; return 0; }
    else return 0;
}

//curve: dummy
fract32 env_tcd_dummy(env_tcd *env) {
    return 0;
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

//curve: one
fract32 env_tcd_one(env_tcd *env) {
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        buffer_head_pos(&(env->head), env->pos);
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

//curve: loop
fract32 env_tcd_loop(env_tcd *env) {
    if (env->state == ON)
    {
        if (!env->countTime)
        {
            env->countTime++;
            buffer_head_pos(&(env->head), env->pos);
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
s32 env_tcd_noise(env_tcd *env) {
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

//curve: record audio, input monitoring
fract32 env_tcd_rec(env_tcd *env) {
    fract32 tmp;
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        tmp = 0;
        tmp = env->input(env);
        buffer_head_pos(&(env->head), env->pos);
        buffer_head_record(&(env->head), tmp);
        return tmp;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        tmp = 0;
        tmp = env->input(env);
        buffer_head_next(&(env->head));
        buffer_head_record(&(env->head), tmp);
        return tmp;
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: record audio, output cv TRIG
fract32 env_tcd_trigrec(env_tcd *env) {
    fract32 tmp;
    if (env->state == ON && (!env->countTime))
    {
        env->countTime++;
        tmp = 0;
        tmp = env->input(env);
        buffer_head_pos(&(env->head), env->pos);
        buffer_head_record(&(env->head), tmp);
        return 0x3fffffff;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        tmp = 0;
        tmp = env->input(env);
        buffer_head_next(&(env->head));
        buffer_head_record(&(env->head), tmp);
        if (env->countTime < 0x0000003f) return 0x3fffffff;
        else return 0;
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: aux mix
fract32 env_tcd_auxmix(env_tcd *env) {
    return aux;
}

//curve: delay
/*
fract32 env_tcd_delay(env_tcd *env) {
    fract32 tmp;
    
    tmp = buffer_head_play(&(env->play));
    if(env->val == 0)
    {
        //  write and replace
        buffer_head_record(&(env->head), env->input(env));
    }
    else if(env->val < 0)
    {
        //  overdub
        buffer_head_dub(&(env->head), env->input(env));
    }
    else
    {
        //  record mix
        buffer_head_mix(&(env->head), env->input(env), env->val);
    }
        
    //advance the phasors
    buffer_head_next(&(env->play));
    buffer_head_next(&(env->head));
    return tmp;
}
*/

//input: physical input 1
fract32 env_tcd_in0(void) {
    return in[0];
}

//input: physical input 2
fract32 env_tcd_in1(void) {
    return in[1];
}

//input: physical inputs 1&2 mix
fract32 env_tcd_in0_1(void) {
    fract32 tmp;
    
    tmp = 0;
    tmp = add_fr1x32(tmp, in[0]);
    tmp = add_fr1x32(tmp, in[1]);
    
    return tmp;
}

//input: aux mix
fract32 env_tcd_aux(void) {
    return aux;
}

//input: track 1 output
fract32 env_tcd_out0(void) {
    return out[0];
}

//input: track 2 output
fract32 env_tcd_out1(void) {
    return out[1];
}

//input: track 3 output
fract32 env_tcd_out2(void) {
    return out[2];
}

//input: track 4 output
fract32 env_tcd_out3(void) {
    return out[3];
}
