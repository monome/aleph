//  env_tcd.c, created by Staffan Jansson on 20/10/14.

#include "env_tcd.h"

//static function declarations
static fract32 (*env_tcd_get_curveptr(u8 n))();
static fract32 (*env_tcd_get_inputptr(u8 n))();

static fract32 env_tcd_off(env_tcd *env);
static fract32 env_tcd_trig(env_tcd *env);
static fract32 env_tcd_loop(env_tcd *env);
static fract32 env_tcd_amp(env_tcd *env);
static fract32 env_tcd_delay(env_tcd *env);
static fract32 env_tcd_thru(env_tcd *env);

static fract32 env_tcd_noinput(void);
static fract32 env_tcd_in0(void);
static fract32 env_tcd_in1(void);
static fract32 env_tcd_in2(void);
static fract32 env_tcd_in3(void);
static fract32 env_tcd_vout0(void);
static fract32 env_tcd_vrev0(void);
static fract32 env_tcd_vout1(void);
static fract32 env_tcd_vrev1(void);
static fract32 env_tcd_vout2(void);
static fract32 env_tcd_vrev2(void);
static fract32 env_tcd_vout3(void);
static fract32 env_tcd_vrev3(void);
static fract32 env_tcd_vout4(void);
static fract32 env_tcd_vrev4(void);
static fract32 env_tcd_vout5(void);
static fract32 env_tcd_vrev5(void);
static fract32 env_tcd_vout6(void);
static fract32 env_tcd_vrev6(void);
static fract32 env_tcd_vout7(void);
static fract32 env_tcd_vrev7(void);
static fract32 env_tcd_aux0(void);
static fract32 env_tcd_arev0(void);
static fract32 env_tcd_aux1(void);
static fract32 env_tcd_arev1(void);
static fract32 env_tcd_amix(void);


//function definitions
void env_tcd_init(env_tcd *env) {
    env->curve = env_tcd_get_curveptr(0);
    env->input = env_tcd_get_inputptr(0);
    env->inL = FR32_MAX;
    env->hdF = 0;
    env->trig = 0;
    env->state = 0;
}


//return pointer to curve algorithm
static fract32 (*env_tcd_get_curveptr(u8 n))() {
    static fract32 (*curves[])() =
    {
        env_tcd_off,            //0
        env_tcd_trig,           //1
        env_tcd_loop,           //2
        env_tcd_amp,            //3
        env_tcd_delay,          //4
        env_tcd_thru,           //5
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
        env_tcd_noinput,
        env_tcd_in0,
        env_tcd_in1,
        env_tcd_in2,
        env_tcd_in3,
        env_tcd_vout0,
        env_tcd_vrev0,
        env_tcd_vout1,
        env_tcd_vrev1,
        env_tcd_vout2,
        env_tcd_vrev2,
        env_tcd_vout3,
        env_tcd_vrev3,
        env_tcd_vout4,
        env_tcd_vrev4,
        env_tcd_vout5,
        env_tcd_vrev5,
        env_tcd_vout6,
        env_tcd_vrev6,
        env_tcd_vout7,
        env_tcd_vrev7,
        env_tcd_aux0,
        env_tcd_arev0,
        env_tcd_aux1,
        env_tcd_arev1,
        env_tcd_amix,
    };
    
    return (n < 1 || n > N_INPUTS) ? *inputs[0] : *inputs[n];
}


//set input
void env_tcd_set_input(env_tcd *env, u8 n) {
    env->input = env_tcd_get_inputptr(n);
}


//set input level
void env_tcd_set_level(env_tcd *env, fract32 level) {
    env->inL = level;
}


//set playhead feedback
void env_tcd_set_feedback(env_tcd *env, fract32 feedback) {
    env->hdF = feedback;
}


//set offsets
void env_tcd_set_start(env_tcd *env, u32 offset) {
    u8 i;
    
    for (i=0; i<N_HEADS; i++)
    {
        env->head[i].start = offset;
    }
}

void env_tcd_set_loop(env_tcd *env, u32 offset) {
    u8 i;
    
    for (i=0; i<N_HEADS; i++)
    {
        env->head[i].end = offset;
    }
}


//trig curve
void env_tcd_set_trig(env_tcd *env) {
    env->trig = 1;
}


//static function definitions
//track: off
fract32 env_tcd_off(env_tcd *env) {
    return 0;
}

//track: wav trig
fract32 env_tcd_trig(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        env->head[0].idx = env->head[0].start;
        return buffer_head_play(&(env->head[0]));
    }
    else
    {
        env->head[0].idx += 1;
        if (env->head[0].idx >= env->head[0].end)
        {
            return 0;
        }
        else
        {
            return buffer_head_play(&(env->head[0]));
        }
    }
}

//track: wav loop
fract32 env_tcd_loop(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        env->head[0].idx = env->head[0].start;
        return buffer_head_play(&(env->head[0]));
    }
    else
    {
        env->head[0].idx += 1;
        if (env->head[0].idx >= env->head[0].end)
        {
            env->trig = 1;
        }
        return buffer_head_play(&(env->head[0]));
    }
}

//track: amp
fract32 env_tcd_amp(env_tcd *env) {
    return 0;
}

//track: dly
fract32 env_tcd_delay(env_tcd *env) {
    fract32 tmp;
    
    if(env->trig)
    {
        env->trig = 0;
        env->head[0].idx = env->head[0].start;
        env->head[1].idx = env->head[1].start;
        
        //  input level attenuation
        tmp = mult_fr1x32x32(buffer_head_play(&(env->head[1])), env->inL);
        
        //  feedback
        buffer_head_mix(&(env->head[0]), env->input(env), env->hdF);
        return tmp;
    }    
    else
    {
        //advance the phasors
        env->head[0].idx += 1;
        env->head[1].idx += 1;
        if (env->head[1].idx >= env->head[1].end)
        {
            env->trig = 1;
        }
        tmp = mult_fr1x32x32(buffer_head_play(&(env->head[1])), env->inL);
        buffer_head_mix(&(env->head[0]), env->input(env), env->hdF);
        return tmp;
    }
}

//curve: thru
fract32 env_tcd_thru(env_tcd *env) {
    return mult_fr1x32x32(env->input(env), env->inL);
}

//curve: noise
s32 env_tcd_noise(env_tcd *env) {
    env->hdF = (u32)env->hdF * 1664525 + 1013904223;
    return (s32) env->hdF;
}

//no input
fract32 env_tcd_noinput(void) {
    return 0;
}

//physical input 1
fract32 env_tcd_in0(void) {
    return in[0];
}

//physical input 2
fract32 env_tcd_in1(void) {
    return in[1];
}

//physical input 3
fract32 env_tcd_in2(void) {
    return in[2];
}

//physical input 4
fract32 env_tcd_in3(void) {
    return in[3];
}

//vout 1
fract32 env_tcd_vout0(void) {
    return vout[0];
}

//vout 1 rev
fract32 env_tcd_vrev0(void) {
    return negate_fr1x32(vout[0]);
}

//vout 2
fract32 env_tcd_vout1(void) {
    return vout[1];
}

//vout 2 rev
fract32 env_tcd_vrev1(void) {
    return negate_fr1x32(vout[1]);
}

//vout 3
fract32 env_tcd_vout2(void) {
    return vout[2];
}

//vout 3 rev
fract32 env_tcd_vrev2(void) {
    return negate_fr1x32(vout[2]);
}

//vout 4
fract32 env_tcd_vout3(void) {
    return vout[3];
}

//vout 4 rev
fract32 env_tcd_vrev3(void) {
    return negate_fr1x32(vout[3]);
}

//vout 5
fract32 env_tcd_vout4(void) {
    return vout[4];
}

//vout 5 rev
fract32 env_tcd_vrev4(void) {
    return negate_fr1x32(vout[4]);
}

//vout 6
fract32 env_tcd_vout5(void) {
    return vout[5];
}

//vout 6 rev
fract32 env_tcd_vrev5(void) {
    return negate_fr1x32(vout[5]);
}

//vout 7
fract32 env_tcd_vout6(void) {
    return vout[6];
}

//vout 7 rev
fract32 env_tcd_vrev6(void) {
    return negate_fr1x32(vout[6]);
}

//vout 8
fract32 env_tcd_vout7(void) {
    return vout[7];
}

//vout 8 rev
fract32 env_tcd_vrev7(void) {
    return negate_fr1x32(vout[7]);
}

//aux 1
fract32 env_tcd_aux0(void) {
    return aux[0];
}

//aux 1 rev
fract32 env_tcd_arev0(void) {
    return negate_fr1x32(aux[0]);
}

//aux 2
fract32 env_tcd_aux1(void) {
    return aux[1];
}

//aux 2 rev
fract32 env_tcd_arev1(void) {
    return negate_fr1x32(aux[1]);
}

//aux mix
fract32 env_tcd_amix(void) {
    return add_fr1x32(aux[0], aux[1]);
}
