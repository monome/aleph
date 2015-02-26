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
static fract32 env_tcd_auxmaster(env_tcd *env);
static fract32 env_tcd_insertmix(env_tcd *env);
static fract32 env_tcd_delay(env_tcd *env);
static fract32 env_tcd_diffuse(env_tcd *env);

static fract32 env_tcd_in0(void);
static fract32 env_tcd_in1(void);
static fract32 env_tcd_in0_1(void);
static fract32 env_tcd_out0(void);
static fract32 env_tcd_out0_6dB(void);
static fract32 env_tcd_out0_12dB(void);
static fract32 env_tcd_out0_rev(void);
static fract32 env_tcd_out0_rev6dB(void);
static fract32 env_tcd_out1(void);
static fract32 env_tcd_out1_6dB(void);
static fract32 env_tcd_out1_12dB(void);
static fract32 env_tcd_out1_rev(void);
static fract32 env_tcd_out1_rev6dB(void);
static fract32 env_tcd_out2(void);
static fract32 env_tcd_out2_6dB(void);
static fract32 env_tcd_out2_12dB(void);
static fract32 env_tcd_out2_rev(void);
static fract32 env_tcd_out2_rev6dB(void);
static fract32 env_tcd_out3(void);
static fract32 env_tcd_out3_6dB(void);
static fract32 env_tcd_out3_12dB(void);
static fract32 env_tcd_out3_rev(void);
static fract32 env_tcd_out3_rev6dB(void);
static fract32 env_tcd_aux(void);
static fract32 env_tcd_aux_rev(void);
static fract32 env_tcd_in_off(void);


//function definitions
void env_tcd_init(env_tcd *env) {
//    u8 i;
    env->curve = env_tcd_get_curveptr(0);
    env->inputA = env_tcd_get_inputptr(0);
    env->inputB = env_tcd_get_inputptr(0);
    env->mix = 0;

    //  env->head is handled in module_init()
/*
    for(i=0; i<N_HEADS; i++)
    {
        buffer[i] = 0;
    }
*/
    env->countTime = 0;
    env->time = 0;
        
    env->trig = 0;
    env->state = OFF;
    env->headState = OFF;
    
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
        env_tcd_auxmaster,      //9
        env_tcd_insertmix,      //10
        env_tcd_delay,          //11
        env_tcd_diffuse,        //12
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
        env_tcd_in0,
        env_tcd_in1,
        env_tcd_in0_1,
        env_tcd_out0,
        env_tcd_out0_6dB,
        env_tcd_out0_12dB,
        env_tcd_out0_rev,
        env_tcd_out0_rev6dB,
        env_tcd_out1,
        env_tcd_out1_6dB,
        env_tcd_out1_12dB,
        env_tcd_out1_rev,
        env_tcd_out1_rev6dB,
        env_tcd_out2,
        env_tcd_out2_6dB,
        env_tcd_out2_12dB,
        env_tcd_out2_rev,
        env_tcd_out2_rev6dB,
        env_tcd_out3,
        env_tcd_out3_6dB,
        env_tcd_out3_12dB,
        env_tcd_out3_rev,
        env_tcd_out3_rev6dB,
        env_tcd_aux,
        env_tcd_aux_rev,
        env_tcd_in_off,
    };
    
    return (n < 1 || n > N_INPUTS) ? *inputs[0] : *inputs[n];
}


//set input A
void env_tcd_set_inputA(env_tcd *env, u8 n) {
    env->inputA = env_tcd_get_inputptr(n);
}


//set input B
void env_tcd_set_inputB(env_tcd *env, u8 n) {
    env->inputB = env_tcd_get_inputptr(n);
}


//set mix
void env_tcd_set_mix(env_tcd *env, fract32 mix) {
    env->mix = mix;
}


//set buffer offsets
void env_tcd_set_offset(env_tcd *env, u32 offset) {
    u8 i;
    
    for (i=0; i<N_HEADS; i++)
    {
        env->head[i].offset = offset;
    }
}


//set buffer loop point
void env_tcd_set_loop(env_tcd *env, u32 loop) {
    u8 i;
    
    for (i=0; i<N_HEADS; i++)
    {
        env->head[i].loop = loop;
    }
}


//set curve time (in relation to offset and position)
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
        buffer_head_pos(&(env->head[1]), env->head[1].offset);
        return buffer_head_play(&(env->head[1]));
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        buffer_head_next(&(env->head[1]));
        return buffer_head_play(&(env->head[1]));
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
            buffer_head_pos(&(env->head[1]), env->head[1].offset);
            return buffer_head_play(&(env->head[1]));
        }
        else if (env->countTime < env->time)
        {
            env->countTime++;
            buffer_head_next(&(env->head[1]));
            return buffer_head_play(&(env->head[1]));
        }
        else
        {
            env->countTime = 0;
            env->countTime++;
            buffer_head_pos(&(env->head[1]), env->head[1].offset);
            return buffer_head_play(&(env->head[1]));
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
            env->mix = (u32)env->mix * 1664525 + 1013904223;
            return (s32) env->mix;
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
        tmp = env->inputA(env);
        buffer_head_pos(&(env->head[0]), env->head[0].offset);
        buffer_head_rec(&(env->head[0]), tmp);
        return tmp;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        tmp = 0;
        tmp = env->inputA(env);
        buffer_head_next(&(env->head[0]));
        buffer_head_rec(&(env->head[0]), tmp);
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
        tmp = env->inputA(env);
        buffer_head_pos(&(env->head[0]), env->head[0].offset);
        buffer_head_rec(&(env->head[0]), tmp);
        return 0x3fffffff;
    }
    else if (env->state == ON && env->countTime < env->time)
    {
        env->countTime++;
        tmp = 0;
        tmp = env->inputA(env);
        buffer_head_next(&(env->head[0]));
        buffer_head_rec(&(env->head[0]), tmp);
        if (env->countTime < 0x0000003f) return 0x3fffffff;
        else return 0;
    }
    else
    {
        env->state = OFF;
        return 0;
    }
}

//curve: aux master
fract32 env_tcd_auxmaster(env_tcd *env) {
    return aux;
}

//curve: insert mix
fract32 env_tcd_insertmix(env_tcd *env) {
    fract32 tmp = mult_fr1x32x32(env->inputA(env), sub_fr1x32(FR32_MAX, env->mix));
    tmp = add_fr1x32(tmp,  mult_fr1x32x32(env->inputB(env), env->mix));
    
//    fract32 tmp = mult_fr1x32x32(env->inputB(env), env->mix);
//    tmp = add_fr1x32(tmp, mult_fr1x32x32(env->inputA(env), sub_fr1x32(FR32_MAX, env->mix)));
    return tmp;
}

//curve: delay
fract32 env_tcd_delay(env_tcd *env) {
    fract32 tmp, input;

    if(!env->countTime)
    {
        env->countTime++;
        buffer_head_pos(&(env->head[0]), env->head[0].offset);
        buffer_head_pos(&(env->head[1]), env->head[1].offset);

        tmp = buffer_head_play(&(env->head[1]));
        input = env->inputA(env);
        
        if(env->mix > 0)
        {
            //  positive feedback
            buffer_head_mix(&(env->head[0]), input, env->mix);
        }
        else if(env->mix < 0)
        {
            //  negative feedback
            buffer_head_invmix(&(env->head[0]), input, env->mix);
        }
        else
        {
            //  write and replace, no feedback
            buffer_head_rec(&(env->head[0]), input);
        }
        
        //advance the phasors
        buffer_head_next(&(env->head[0]));
        buffer_head_next(&(env->head[1]));
        
        return tmp;
    }

    else
    {        
        tmp = buffer_head_play(&(env->head[1]));
        input = env->inputA(env);
        
        if(env->mix > 0)
        {
            //  positive feedback
            buffer_head_mix(&(env->head[0]), input, env->mix);
        }
        else if(env->mix < 0)
        {
            //  negative feedback
            buffer_head_invmix(&(env->head[0]), input, env->mix);
        }
        else
        {
            //  write and replace, no feedback
            buffer_head_rec(&(env->head[0]), input);
        }
        
        //advance the phasors
        buffer_head_next(&(env->head[0]));
        buffer_head_next(&(env->head[1]));
        
        return tmp;
    }
}

//curve: diffuse
fract32 env_tcd_diffuse(env_tcd *env) {
    return 0;
}

/*
    fract32 output;
    
    fract32 ir[N_HEADS], history[N_HEADS];
    fract32 *h = history;
    fract32 *tmp;
    
    output = 0;
    *h = env->inputA(env);
    tmp = h;
    for(i=0, i < N_HEADS - 1; i++)
    {
        output = output + ir[i] * history[tmp];
        tmp--;
        if(tmp<0) tmp = N_HEADS - 1;
    }
    h++;
    if (h == N_HEADS) h = 0;
    
    return output;
    
    
    
    fract32 tmp;
    
    if(!env->countTime)
    {
        env->countTime++;
        
        for (i=0; i>N_HEADS; i++)
        {
            buffer_head_pos(&(env->head[i]), env->head[i].offset);
        }
        
        env->head[1].loop *= 0.895;
        env->head[1].mix = mult_fr1x32x32(0.75, FR32_MAX);
        
        env->head[2].loop *= 0.665;
        env->head[2].mix = mult_fr1x32x32(0.72, FR32_MAX);

        env->head[3].loop *= 0.475;
        env->head[3].mix = mult_fr1x32x32(0.691, FR32_MAX);

        env->head[4].loop *= 0.357; //check!
        env->head[4].mix = mult_fr1x32x32(0.646, FR32_MAX);

        env->head[5].loop *= 0.22; //check!
        env->head[5].mix = mult_fr1x32x32(0.666, FR32_MAX);

        env->head[6].loop *= x;
        env->head[6].mix = mult_fr1x32x32(0.75, FR32_MAX);

        env->head[7].loop *= y;
        env->head[7].mix = mult_fr1x32x32(0.75, FR32_MAX);

    }
    
    else
    {
        env->history[0] = env->inputA(env);
        
        buffer_head_mix(&(env->head[1]), env->history[0], env->mix[1]);

        
        
        
        
 
        
        
        

        return head->buf->data[head->idx];

        buffer_head_play(env->head[1]);
        
        
        
        
        
    }
        
        
        
        
        
        

    
    
    
    
    
    
    
    u8 i = 1;
    fract32 tmp, input;

        tmp = buffer_head_play(&(env->head[1]));
        tmp = add_fr1x32(tmp, buffer_head_play(&(env->head[2])));
        tmp = add_fr1x32(tmp, buffer_head_play(&(env->head[3])));
        tmp = add_fr1x32(tmp, buffer_head_play(&(env->head[4])));
        input = env->inputA(env);

        if(env->mix > 0)
        {
            //  positive feedback
            buffer_head_mix(&(env->head[0]), input, env->mix);
        }
        else if(env->mix < 0)
        {
            //  negative feedback
            buffer_head_invmix(&(env->head[0]), input, env->mix);
        }
        else
        {
            //  write and replace, no feedback
            buffer_head_rec(&(env->head[0]), input);
        }
        
        //  advance the phasors
        for(i=0; i<N_HEADS; i++)
        {
            buffer_head_next(&(env->head[i]));
        }
        
        return tmp;
    }
    else
    {
        tmp = buffer_head_play(&(env->head[1]));
        tmp = add_fr1x32(tmp, buffer_head_play(&(env->head[2])));
        tmp = add_fr1x32(tmp, buffer_head_play(&(env->head[3])));
        tmp = add_fr1x32(tmp, buffer_head_play(&(env->head[4])));
        input = env->inputA(env);
        
        if(env->mix > 0)
        {
            //  positive feedback
            buffer_head_mix(&(env->head[0]), input, env->mix);
        }
        else if(env->mix < 0)
        {
            //  negative feedback
            buffer_head_invmix(&(env->head[0]), input, env->mix);
        }
        else
        {
            //  write and replace, no feedback
            buffer_head_rec(&(env->head[0]), input);
        }
        
        //advance the phasors
        for(i=0; i<N_HEADS; i++)
        {
            buffer_head_next(&(env->head[i]));
        }

        return tmp;
    }
}

//    bitfield?

    s32 swap_s32 (s32 val) {
        //load union with u8 trigs
        //            ctl_param_change(eParamTrig, (1 >> 24) & 0xff);
        //            ctl_param_change(eParamTrig, (1 >> 16) & 0xff);
        //            ctl_param_change(eParamTrig, (1 >> 8) & 0xff);
        //            ctl_param_change(eParamTrig, 1 & 0xff);
        
        //byteswap union
        return ((val << 24) & 0xff000000) | ((val << 8) & 0x00ff0000) | ((val >> 8) & 0x0000ff00) | ((val >> 24) & 0x000000ff);

        //    p = data->trig[c];
        //    env_tcd_set_trig(&(track[0]->envAmp), (p >> 24) & 0xff);
        //    env_tcd_set_trig(&(track[1]->envAmp), (p >> 16) & 0xff);
        //    env_tcd_set_trig(&(track[2]->envAmp), (p >> 8) & 0xff);
        //    env_tcd_set_trig(&(track[3]->envAmp), p & 0xff);
        
        //    return ((val&0xff)<<24)+((val&0xff00)<<8)+((val&0xff0000)>>8)+((val>>24)&0xff);
    }



//curve: ir
fract32 env_tcd_ir(env_tcd *env) {
    fract32 output;
    
    fract32 ir[N_HEADS], history[N_HEADS];
    fract32 *h = history;
    fract32 *tmp;
    
    output = 0;
    *h = env->inputA(env);
    tmp = h;
    for(i=0, i < N_HEADS - 1; i++)
    {
        output = output + ir[i] * history[tmp];
        tmp--;
        if(tmp<0) tmp = N_HEADS - 1;
    }
    h++;
    if (h == N_HEADS) h = 0;
    
    return output;
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

//input: track 1 output
fract32 env_tcd_out0(void) {
    return out[0];
}

//input: track 1 output -6db
fract32 env_tcd_out0_6dB(void) {
    fract32 tmp;
    
    tmp = out[0];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 1);
    
    return tmp;
}

//input: track 1 output -12db
fract32 env_tcd_out0_12dB(void) {
    fract32 tmp;
    
    tmp = out[0];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 2);
    
    return tmp;
}

//input: track 1 output polarity reverse
fract32 env_tcd_out0_rev(void) {
    fract32 tmp;
    
    tmp = out[0];
    tmp = negate_fr1x32(tmp);
    
    return tmp;
}

//input: track 1 output reverse -6dB
fract32 env_tcd_out0_rev6dB(void) {
    fract32 tmp;
    
    tmp = out[0];
    tmp = mult_fr1x32x32(negate_fr1x32(tmp), FR32_MAX >> 1);
    
    return tmp;
}

//input: track 2 output
fract32 env_tcd_out1(void) {
    return out[1];
}

//input: track 2 output -6db
fract32 env_tcd_out1_6dB(void) {
    fract32 tmp;
    
    tmp = out[1];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 1);
    
    return tmp;
}

//input: track 2 output -12db
fract32 env_tcd_out1_12dB(void) {
    fract32 tmp;
    
    tmp = out[1];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 2);
    
    return tmp;
}

//input: track 2 output polarity reverse
fract32 env_tcd_out1_rev(void) {
    fract32 tmp;
    
    tmp = out[1];
    tmp = negate_fr1x32(tmp);
    
    return tmp;
}

//input: track 2 output reverse -6dB
fract32 env_tcd_out1_rev6dB(void) {
    fract32 tmp;
    
    tmp = out[1];
    tmp = mult_fr1x32x32(negate_fr1x32(tmp), FR32_MAX >> 1);
    
    return tmp;
}

//input: track 3 output
fract32 env_tcd_out2(void) {
    return out[2];
}

//input: track 2 output -6db
fract32 env_tcd_out2_6dB(void) {
    fract32 tmp;
    
    tmp = out[2];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 1);
    
    return tmp;
}

//input: track 3 output -12db
fract32 env_tcd_out2_12dB(void) {
    fract32 tmp;
    
    tmp = out[2];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 2);
    
    return tmp;
}

//input: track 3 output polarity reverse
fract32 env_tcd_out2_rev(void) {
    fract32 tmp;
    
    tmp = out[2];
    tmp = negate_fr1x32(tmp);

    return tmp;
}

//input: track 2 output reverse -6dB
fract32 env_tcd_out2_rev6dB(void) {
    fract32 tmp;
    
    tmp = out[2];
    tmp = mult_fr1x32x32(negate_fr1x32(tmp), FR32_MAX >> 1);
    
    return tmp;
}

//input: track 4 output
fract32 env_tcd_out3(void) {
    return out[3];
}

//input: track 2 output -6db
fract32 env_tcd_out3_6dB(void) {
    fract32 tmp;
    
    tmp = out[3];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 1);
    
    return tmp;
}

//input: track 4 output -12db
fract32 env_tcd_out3_12dB(void) {
    fract32 tmp;
    
    tmp = out[3];
    tmp = mult_fr1x32x32(tmp, FR32_MAX >> 2);
    
    return tmp;
}

//input: track 4 output polarity reverse
fract32 env_tcd_out3_rev(void) {
    fract32 tmp;
    
    tmp = out[3];
    tmp = negate_fr1x32(tmp);
    
    return tmp;
}

//input: track 2 output reverse -6dB
fract32 env_tcd_out3_rev6dB(void) {
    fract32 tmp;
    
    tmp = out[3];
    tmp = mult_fr1x32x32(negate_fr1x32(tmp), FR32_MAX >> 1);
    
    return tmp;
}

//input: aux mix
fract32 env_tcd_aux(void) {
    return aux;
}

//input: aux mix polarity reverse
fract32 env_tcd_aux_rev(void) {
    fract32 tmp;
    
    tmp = aux;
    tmp = negate_fr1x32(tmp);
    
    return tmp;
}

//input:off
fract32 env_tcd_in_off(void) {
    return 0;
}
