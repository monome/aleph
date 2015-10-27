//  env_tcd.c, created by Staffan Jansson on 20/10/14.
//  Interpolation algorithms based on "Polynomial Interpolators for High-
//  Quality Resampling of Oversampled Audio" by Olli Niemitalo

#include "env_tcd.h"

//static function declarations
static fract32 (*env_tcd_get_curveptr(u8 n))();
static fract32 (*env_tcd_get_inputptr(u8 n))();

static fract32 env_tcd_off(env_tcd *env);

static fract32 env_tcd_trig(env_tcd *env);

static fract32 env_tcd_trigbus(env_tcd *env);

static fract32 env_loop_linear(env_tcd *env);
static fract32 env_loop_test(env_tcd *env);
static fract32 env_loop_Rtest(env_tcd *env);

static fract32 env_tape_fwd(env_tcd *env);
static fract32 env_tape_rvs(env_tcd *env);
static fract32 env_scrub_fwd(env_tcd *env);
static fract32 env_scrub_rvs(env_tcd *env);

static fract32 env_amp_lindec(env_tcd *env);
static fract32 env_amp_expodec(env_tcd *env);
static fract32 env_amp_Rexpodec(env_tcd *env);
static fract32 env_amp_tri(env_tcd *env);
static fract32 env_amp_gauss(env_tcd *env);

static fract32 env_aux_delay(env_tcd *env);
static fract32 env_aux_input(env_tcd *env);

static fract32 env_tcd_noinput(void);
static fract32 env_tcd_in0(void);
static fract32 env_tcd_in1(void);
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
static fract32 env_tcd_aux(void);
static fract32 env_tcd_arev(void);


//function definitions
void env_tcd_init(env_tcd *env) {
    env->curve = env_tcd_get_curveptr(0);
    env->input = env_tcd_get_inputptr(0);
    env->inL = FR32_MAX;
    env->hdF = 0;
    env->decay = 0;
    env->x = 0;
    env->c0 = 0;
    env->c1 = 0;
    env->c2 = 0;
    env->c3 = 0;
    env->env = 0;
    env->trig = 0;
    env->state = 0;
    env->motor = 0;
}


//return pointer to curve algorithm
static fract32 (*env_tcd_get_curveptr(u8 n))() {
    static fract32 (*curves[])() =
    {
        env_tcd_off,            //0
        
        env_tcd_trig,           //1
        
        env_tcd_trigbus,        //2
                
        env_loop_linear,        //3
        env_loop_test,          //4
        env_loop_Rtest,         //5

        env_tape_fwd,           //6
        env_tape_rvs,           //7
        env_scrub_fwd,          //8
        env_scrub_rvs,          //9
        
        env_amp_lindec,         //10
        env_amp_expodec,        //11
        env_amp_Rexpodec,       //12
        env_amp_tri,            //13
        env_amp_gauss,          //14
        
        env_aux_delay,          //15
        env_aux_input,          //16
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
        env_tcd_aux,
        env_tcd_arev,
        env_tcd_in0,
        env_tcd_in1,
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


//set decay
extern void env_tcd_set_decay(env_tcd *env, fract32 decay) {
    env->decay = decay;
}


//trig curve
void env_tcd_set_trig(env_tcd *env) {
    env->trig = 1;
}


//set motor state
void env_tcd_set_motor(env_tcd *env, u8 state) {
    env->motor = state;
}


//static function definitions
//track: off
fract32 env_tcd_off(env_tcd *env) {
    return 0;
}

//track: wav trig
//ADD DECAY and PITCH to TRIG MODE as default, using parameter routing, route_param()
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

//track: wav trig - level ctrl
/*
 fixed parameters: pitch, attack, decay
*/
//ADD A-D ENVELOPE WITH FIXED PARAMETERS, COMPRESSED ENVELOPE
//oversampling 2x
// process 2x and average
/*
 fract32 out = shr_fr1x32(filter_svf_calc_frame(f, in), 1);
 out = add_fr1x32(out, shr_fr1x32(filter_svf_calc_frame(f, in), 1));
 return out;
 */
//Linear
/*
 env->x += env->decay;
 env->head[0].idx = add_fr1x32(env->head[0].start, mult_fr1x32x32(env->x, sub_fr1x32(env->head[0].end, env->head[0].start)));
 */
fract32 env_lvl_trig(env_tcd *env) {
    if (env->trig)
        //ATTACK START 0
        //a ATTACK START 1, DECAY START 0
        //b ATTACK START 0, DECAY START 1
    {
        env->trig = 0;
        env->head[0].idx = env->head[0].start;
        return mult_fr1x32x32(buffer_head_play(&(env->head[0])), env->decay); //rename decay to sP/common name
    }
    //ELSE SAMPLE REACHES ENDPOINT OR ZERO, RESTART AT ATTACK START 0
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



//track: wav trig - pitch ctrl
/*
 fixed parameters: level, attack, decay
 linear 8x oversampling with pre-emphasis
 
 1 + x^2*-0.00129654618588707
*/
 
//track: wav trig - attack ctrl
/*
 fixed parameters: level, pitch, decay
*/

//track: wav trig - decay ctrl
/*
 fixed parameters: level, pitch, attack
*/



//TEST
fract32 env_tcd_trigbus(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        return 0x7fffffff;
    }
    else
    {
        return 0;
    }
}

fract32 env_loop_linear(env_tcd *env) {
    return 0;
}

fract32 env_loop_test(env_tcd *env) {
    return 0;
}

fract32 env_loop_Rtest(env_tcd *env) {
    return 0;
}

fract32 env_tape_fwd(env_tcd *env) {
    if (env->motor)
    {
        if (env->trig)
        {
            env->trig = 0;
            env->x = 0;
            
            //  perform pre-calculations, return first sample
            fract32 loop = sub_fr1x32(env->head[0].end, env->head[0].start);
            fract32 y1n = sub_fr1x32(env->head[0].start, loop);
            fract32 y0 = env->head[0].start;
            fract32 y1 = env->head[0].end;
            fract32 y2 = add_fr1x32(env->head[0].end, loop);
            fract32 e1 = add_fr1x32(y1n, y2), o1 = sub_fr1x32(y2, y1n);
            fract32 e2 = add_fr1x32(y0, y1), o2 = sub_fr1x32(y1, y0);
            env->c0 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_48, e1), mult_fr1x32x32(FR32_MAX23_48, e2));
            env->c1 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_8, o1), mult_fr1x32x32(FR32_MAX5_8, o2));
            env->c2 = mult_fr1x32x32(FR32_MAX1_4, sub_fr1x32(e1, e2));
            env->c3 = sub_fr1x32(mult_fr1x32x32(FR32_MAX1_6, o1), mult_fr1x32x32(FR32_MAX1_2, o2));
            env->head[0].idx = y0;
            return buffer_head_play(&(env->head[0]));
        }
        else
        {
            //  return interpolated sample
            env->x += env->decay;
            fract32 z = sub_fr1x32(env->x, FR32_MAX1_2);
            env->head[0].idx = add_fr1x32(env->c0, mult_fr1x32x32(z, add_fr1x32(env->c1, mult_fr1x32x32(z, add_fr1x32(env->c2, mult_fr1x32x32(z, env->c3))))));
            if (env->head[0].idx >= env->head[0].end - 0xf)
            {
                env->x = 0;
                env->head[0].idx = env->head[0].start;
            }
            return buffer_head_play(&(env->head[0]));
        }
    }
    else return 0;
}

fract32 env_tape_rvs(env_tcd *env) {
    if (env->motor)
    {
        if (env->trig)
        {
            env->trig = 0;
            env->x = 0;
            
            //  perform pre-calculations, return first sample
            fract32 loop = sub_fr1x32(env->head[0].end, env->head[0].start);
            fract32 y1n = sub_fr1x32(env->head[0].start, loop);
            fract32 y0 = env->head[0].start;
            fract32 y1 = env->head[0].end;
            fract32 y2 = add_fr1x32(env->head[0].end, loop);
            fract32 e1 = add_fr1x32(y1n, y2), o1 = sub_fr1x32(y2, y1n);
            fract32 e2 = add_fr1x32(y0, y1), o2 = sub_fr1x32(y1, y0);
            env->c0 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_48, e1), mult_fr1x32x32(FR32_MAX23_48, e2));
            env->c1 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_8, o1), mult_fr1x32x32(FR32_MAX5_8, o2));
            env->c2 = mult_fr1x32x32(FR32_MAX1_4, sub_fr1x32(e1, e2));
            env->c3 = sub_fr1x32(mult_fr1x32x32(FR32_MAX1_6, o1), mult_fr1x32x32(FR32_MAX1_2, o2));
            env->head[0].idx = y1;
            return buffer_head_play(&(env->head[0]));
        }
        else
        {
            //  return interpolated sample
            env->x -= env->decay;
            fract32 z = sub_fr1x32(FR32_MAX1_2, env->x);
            env->head[0].idx = add_fr1x32(env->c0, mult_fr1x32x32(z, add_fr1x32(env->c1, mult_fr1x32x32(z, add_fr1x32(env->c2, mult_fr1x32x32(z, env->c3))))));
            if (env->head[0].idx <= env->head[0].start + 0xf)
            {
                env->x = 0;
                env->head[0].idx = env->head[0].end;
            }
            return buffer_head_play(&(env->head[0]));
        }
    }
    else return 0;
}

fract32 env_scrub_fwd(env_tcd *env) {
//    if (!env->motor)
//    {
        if (env->trig)
        {
            env->trig = 0;
            env->x = env->decay;
            
            //  perform pre-calculations, return first sample
            fract32 loop = sub_fr1x32(env->head[0].end, env->head[0].start);
            fract32 y1n = sub_fr1x32(env->head[0].start, loop);
            fract32 y0 = env->head[0].start;
            fract32 y1 = env->head[0].end;
            fract32 y2 = add_fr1x32(env->head[0].end, loop);
            fract32 e1 = add_fr1x32(y1n, y2), o1 = sub_fr1x32(y2, y1n);
            fract32 e2 = add_fr1x32(y0, y1), o2 = sub_fr1x32(y1, y0);
            env->c0 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_48, e1), mult_fr1x32x32(FR32_MAX23_48, e2));
            env->c1 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_8, o1), mult_fr1x32x32(FR32_MAX5_8, o2));
            env->c2 = mult_fr1x32x32(FR32_MAX1_4, sub_fr1x32(e1, e2));
            env->c3 = sub_fr1x32(mult_fr1x32x32(FR32_MAX1_6, o1), mult_fr1x32x32(FR32_MAX1_2, o2));
            env->head[0].idx = y0;
            return buffer_head_play(&(env->head[0]));
        }
        else
        {
//            fract32 tmp = env->decay;
//            tmp = sub_fr1x32(mult_fr1x32x32(FR32_MAX, sub_fr1x32(FR32_MAX, tmp)), mult_fr1x32x32(FR32_MAX, tmp));
//            env->x += tmp;
            env->x += mult_fr1x32x32(FR32_MAX, env->decay);
            fract32 z = sub_fr1x32(env->x, FR32_MAX1_2);
            env->head[0].idx = add_fr1x32(env->c0, mult_fr1x32x32(z, add_fr1x32(env->c1, mult_fr1x32x32(z, add_fr1x32(env->c2, mult_fr1x32x32(z, env->c3))))));
            if (env->head[0].idx >= env->head[0].end)
            {
                env->x = 0;
                env->head[0].idx = env->head[0].start;
            }
            return buffer_head_play(&(env->head[0]));
        }
}

fract32 env_scrub_rvs(env_tcd *env) {
//    if (!env->motor)
//    {
        if (env->trig)
        {
            env->trig = 0;
            env->x = 0;
            
            //  perform pre-calculations, return first sample
            fract32 loop = sub_fr1x32(env->head[0].end, env->head[0].start);
            fract32 y1n = sub_fr1x32(env->head[0].start, loop);
            fract32 y0 = env->head[0].start;
            fract32 y1 = env->head[0].end;
            fract32 y2 = add_fr1x32(env->head[0].end, loop);
            fract32 e1 = add_fr1x32(y1n, y2), o1 = sub_fr1x32(y2, y1n);
            fract32 e2 = add_fr1x32(y0, y1), o2 = sub_fr1x32(y1, y0);
            env->c0 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_48, e1), mult_fr1x32x32(FR32_MAX23_48, e2));
            env->c1 = add_fr1x32(mult_fr1x32x32(FR32_MAX1_8, o1), mult_fr1x32x32(FR32_MAX5_8, o2));
            env->c2 = mult_fr1x32x32(FR32_MAX1_4, sub_fr1x32(e1, e2));
            env->c3 = sub_fr1x32(mult_fr1x32x32(FR32_MAX1_6, o1), mult_fr1x32x32(FR32_MAX1_2, o2));
            env->head[0].idx = y1;
            return buffer_head_play(&(env->head[0]));
        }
        else
        {
//            fract32 tmp = env->decay;
//            tmp = add_fr1x32(mult_fr1x32x32(FR32_MAX, sub_fr1x32(FR32_MAX, tmp)), mult_fr1x32x32(FR32_MAX, tmp));
//            env->x -= tmp;
            env->x -= mult_fr1x32x32(FR32_MAX, env->decay);
            fract32 z = sub_fr1x32(FR32_MAX1_2, env->x);
            env->head[0].idx = add_fr1x32(env->c0, mult_fr1x32x32(z, add_fr1x32(env->c1, mult_fr1x32x32(z, add_fr1x32(env->c2, mult_fr1x32x32(z, env->c3))))));
            if (env->head[0].idx <= env->head[0].start)
            {
                env->x = 0;
                env->head[0].idx = env->head[0].end;
            }
            return buffer_head_play(&(env->head[0]));
        }
//    }
//    else return 0;
}

//track: amp
fract32 env_amp_lindec(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        env->x = 0;
        return env->input(env);
    }
    else
    {
        env->x += env->decay;
        env->env = sub_fr1x32(FR32_MAX, env->x);
//        return mult_fr1x32x32(mult_fr1x32x32(env->input(env), env->inL), env->env);
        return mult_fr1x32x32(env->input(env), env->env);

    }
}

fract32 env_amp_expodec(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        env->env = FR32_MAX;
        env->x = 0;
        return env->input(env);
    }
    else
    {
        env->x += env->decay;
        env->env = mult_fr1x32x32(env->env, sub_fr1x32(FR32_MAX, env->x));
//        return mult_fr1x32x32(mult_fr1x32x32(env->input(env), env->inL), env->env);
        return mult_fr1x32x32(env->input(env), env->env);
    }
}

fract32 env_amp_Rexpodec(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        env->env = 0;
        env->x = 0;
        return 0;
    }
    else
    {
        env->x += env->decay;
        env->env = add_fr1x32(env->env, mult_fr1x32x32(FR32_MAX, negate_fr1x32(env->x)));
//        return mult_fr1x32x32(mult_fr1x32x32(env->input(env), env->inL), env->env);
        return mult_fr1x32x32(env->input(env), env->env);
    }
}

fract32 env_amp_tri(env_tcd *env) {
    if (env->trig)
    {
        env->trig = 0;
        env->x = 0;
        return 0;
    }
    else
    {
        env->x += env->decay;
        env->env = abs_fr1x32(mult_fr1x32x32(FR32_MAX, negate_fr1x32(env->x)));
//        return mult_fr1x32x32(mult_fr1x32x32(env->input(env), env->inL), env->env);
        return mult_fr1x32x32(env->input(env), env->env);
    }
}

fract32 env_amp_gauss(env_tcd *env) {
    return 0;
}

//track: dly
fract32 env_aux_delay(env_tcd *env) {
//    fract32 tmp;
    
    if(env->trig)
    {
        env->trig = 0;
        env->head[0].idx = env->head[0].start;
        env->head[1].idx = env->head[1].start;
        
        //  input level attenuation
//        tmp = mult_fr1x32x32(buffer_head_play(&(env->head[1])), env->inL);
        
        //  feedback
        buffer_head_mix(&(env->head[0]), env->input(env), env->hdF);
//        return tmp;
        return buffer_head_play(&(env->head[1]));
    }    
    else
    {
        //advance the phasors
        env->head[0].idx += 1;
        env->head[1].idx += 1;
        if (env->head[1].idx >= env->head[1].end)
        {
            //env->trig = 1;
            env->head[0].idx = env->head[0].start;
            env->head[1].idx = env->head[1].start;
        }
//        tmp = mult_fr1x32x32(buffer_head_play(&(env->head[1])), env->inL);
        buffer_head_mix(&(env->head[0]), env->input(env), env->hdF);
//        return tmp;
        return buffer_head_play(&(env->head[1]));
    }
}

//track: input
fract32 env_aux_input(env_tcd *env) {
    return mult_fr1x32x32(env->input(env), env->inL);
}

//no input
fract32 env_tcd_noinput(void) {
    return 0;
}

//aux
fract32 env_tcd_aux(void) {
    return aux;
}

//aux rev
fract32 env_tcd_arev(void) {
    return negate_fr1x32(aux);
}

//physical input 1
fract32 env_tcd_in0(void) {
    return in[0];
}

//physical input 2
fract32 env_tcd_in1(void) {
    return in[1];
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
