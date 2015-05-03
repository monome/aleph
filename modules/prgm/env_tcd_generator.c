//  env_tcd_generator.c
//  aleph

#include "env_tcd_generator.h"

/*
// set release duration in samples
void env_asr_set_rel_dur(env_asr* env, u32 dur) {
    env->bDn = float_to_fr32( (float)( pow(ENV_MIN_D, 1.0 / (double)dur) ) );
    env->rDn = FR32_MAX / dur;
    //  env->aDn = 1.0 / env->bUp;
}

fract32 gen_lindec(gen* env) {
    fract32 cOld = env->cDn;
    env->cDn = c;
    if((cOld < 0) != (c < 0)) {
        if (c < 0) {
            env->stateFP = &(env_next_rel_neg);
        } else {
            env->stateFP = &(env_next_rel_pos);
        }
        if(env->state == ENV_STATE_REL) {
            // invert current phase
            env->y = sub_fr1x32(FR32_MAX, env->y);
            env->x = sub_fr1x32(FR32_MAX, env->x);
        }
    }
}


static fract32 bandlimit_frame(bandlimit *bl, fract32 input);
*/

/*
 fract32 env_tcd_insertmix(env_tcd *env) {
 fract32 tmp = mult_fr1x32x32(env->inputA(env), sub_fr1x32(FR32_MAX, env->mix));
 tmp = add_fr1x32(tmp,  mult_fr1x32x32(env->inputB(env), env->mix));
 return tmp;
 */


 /*
 fract32 pf_audio_envlevel(prgmTrack *t) {
 if(t->envAmp.state == OFF)
 {
 return 0;
 }
 else
 {
 //SDRAM -> ONCHIP copy/paste function
 //onchip playhead?
 //separate lookup algorithm, onchip, copy from waves
 //env time = loop time
 //apply env using onchip buffer here
 return t->envAmp.curve(&(t->envAmp));
 //        return mult_fr1x32x32(t->envAmp.curve(&(t->envAmp)), t->pL);
 //    }
 }
 //LEVEL = (TRACK MIX SEND + ENV OUTPUT) * TRACK MIX PAN LEFT
 //tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], LEVEL));
 //        tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], track[i]->panR));
 
 
 //process 3: aux | mix
 fract32 pf_aux_mix(prgmTrack *t) {
 //    env_tcd_set_mix(&(t->envAmp), t->pM);
 //    if(t->envAmp.state == OFF)
 //    {
 return 0;
 //    }
 //    else
 //    {
 //        return t->envAmp.curve(&(t->envAmp));
 //    }
 }
 
 
 //process 4: aux | mix, loop point
 fract32 pf_aux_mixloop(prgmTrack *t) {
 //    env_tcd_set_mix(&(t->envAmp), t->pM);
 //    filter_1p_lo_in(&(t->eSlew), t->eD);
 
 //  output sequenced value
 //    if(filter_1p_sync(&(t->eSlew)))
 //    {
 //        return t->envAmp.curve(&(t->envAmp));
 //    }
 //    //  output encoder value
 //    else
 //    {
 //        env_tcd_set_loop(&(t->envAmp), t->len);
 return t->envAmp.curve(&(t->envAmp));
 //    }
 }
 
 
 //process 5: none | cv
 fract32 pf_cv(prgmTrack *t) {
 return 0;
 }
 
 
 //process 6: cv | frequency
 fract32 pf_cv_frq(prgmTrack *t) {
 //    filter_1p_lo_in(&(t->pSlew), t->pF);
 return filter_1p_lo_next(&(t->eSlew));
 
 //    filter_1p_lo_in(&(t->pSlew), t->pF);
 if(filter_1p_sync(&(t->pSlew)))
 {
 //output curve
 //        bandlimit_set_f(&(t->envAmp.bpf), t->pF);
 return t->envAmp.curve(&(t->envAmp));
 }
 else
 {
 //output encoder
 //        bandlimit_set_f(&(t->envAmp.bpf), filter_1p_lo_next(&(t->pSlew)));
 return t->envAmp.curve(&(t->envAmp));
 }
 }
 
 
 
 //envelope frame
 void set_envelope(prgmTrack *t, u8 n) {
 //todo...
 //envelope offset (shape)
 //envelope decay/duration
 //envelope level
 }
 */

/*
 
 
 void play_step(u32 c) {
 if (sq[0]->tg[c])
 {
 track_set_output(track[0], sq[0]->l[c]);
 env_tcd_set_loop(&(track[0]->envAmp), sq[0]->b[c]);
 env_tcd_set_start(&(track[0]->envAmp), sq[0]->a[c]);
 env_tcd_set_trig(&(track[0]->envAmp));
 }
 if (sq[1]->tg[c])
 {
 track_set_output(track[1], sq[1]->l[c]);
 env_tcd_set_loop(&(track[1]->envAmp), sq[1]->b[c]);
 env_tcd_set_start(&(track[1]->envAmp), sq[1]->a[c]);
 env_tcd_set_trig(&(track[1]->envAmp));
 }
 if (sq[2]->tg[c])
 {
 track_set_output(track[2], sq[2]->l[c]);
 env_tcd_set_loop(&(track[2]->envAmp), sq[2]->b[c]);
 env_tcd_set_start(&(track[2]->envAmp), sq[2]->a[c]);
 env_tcd_set_trig(&(track[2]->envAmp));
 }
 if (sq[3]->tg[c])
 {
 track_set_output(track[3], sq[3]->l[c]);
 env_tcd_set_loop(&(track[3]->envAmp), sq[3]->b[c]);
 env_tcd_set_start(&(track[3]->envAmp), sq[3]->a[c]);
 env_tcd_set_trig(&(track[3]->envAmp));
 }
 if (sq[4]->tg[c])
 {
 track_set_output(track[4], sq[4]->l[c]);
 env_tcd_set_loop(&(track[4]->envAmp), sq[4]->b[c]);
 env_tcd_set_start(&(track[4]->envAmp), sq[4]->a[c]);
 env_tcd_set_trig(&(track[4]->envAmp));
 }
 if (sq[5]->tg[c])
 {
 track_set_output(track[5], sq[5]->l[c]);
 env_tcd_set_loop(&(track[5]->envAmp), sq[5]->b[c]);
 env_tcd_set_start(&(track[5]->envAmp), sq[5]->a[c]);
 env_tcd_set_trig(&(track[5]->envAmp));
 }
 if (sq[6]->tg[c])
 {
 track_set_output(track[6], sq[6]->l[c]);
 env_tcd_set_loop(&(track[6]->envAmp), sq[6]->b[c]);
 env_tcd_set_start(&(track[6]->envAmp), sq[6]->a[c]);
 env_tcd_set_trig(&(track[6]->envAmp));
 }
 if (sq[7]->tg[c])
 {
 track_set_output(track[7], sq[7]->l[c]);
 env_tcd_set_loop(&(track[7]->envAmp), sq[7]->b[c]);
 env_tcd_set_start(&(track[7]->envAmp), sq[7]->a[c]);
 env_tcd_set_trig(&(track[7]->envAmp));
 }
 }
*/



































/*
void bandlimit_init(bandlimit *bl) {
    bl->f = 0;
    bl->q = 0x007cffff; //TEST!!!
    bl->lp = bl->hp = bl->bp = 0;
}


void bandlimit_set_f(bandlimit *bl, fract32 f) {
    if(bl->f != f) bl->f = f;
}


fract32 bandlimit_next(bandlimit *bl, fract32 input) {
    //process x2 and average
    fract32 tmp = shr_fr1x32(bandlimit_frame(bl, input), 1);
    tmp = add_fr1x32(tmp, shr_fr1x32(bandlimit_frame(bl, input), 1));
    return tmp;
}


fract32 bandlimit_frame(bandlimit *bl, fract32 input) {
    bl->lp = add_fr1x32(bl->lp, mult_fr1x32x32(bl->f, bl->bp));
    bl->hp = sub_fr1x32(sub_fr1x32(input, mult_fr1x32x32(bl->q, bl->bp)), bl->lp);
    bl->bp = add_fr1x32(bl->bp, mult_fr1x32x32(bl->f, bl->hp));
    return bl->bp;
}
*/
