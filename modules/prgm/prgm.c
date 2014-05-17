//prgm.c
//aleph-bfin

//standard libraries
#include <math.h>
#include <stdlib.h>
#include <string.h>

//aleph-common
#include "fix.h"
#include "slew.h"
#include "types.h"

//audiolib
#include "filter_1p.h"
#include "interpolate.h"
#include "table.h"
#include "conversion.h"

//bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "params.h"
#include "prgm.h"


//data types
typedef struct _prgmData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} prgmData;

ModuleData *gModuleData;

prgmOscillator *oscillator[N_OSCILLATORS];

static prgmData *data;

static const fract32 wavtab[WAVE_SHAPE_NUM][WAVE_TAB_SIZE] = { 
#include "wavtab_data_inc.c" 
};

static u32      sr;     //sample rate
//static fix16    ips;    //index change per sample

static fract32 ioAmp0;
static fract32 ioAmp1;
static fract32 ioAmp2;
static fract32 ioAmp3;


//static function declaration
static inline fract32 freq_to_phase(fix16 freq);

static inline void oscillator_calc_inc(prgmOscillator *oscillator);

PrgmOscillatorpointer init(void);

static void init_parameters(prgmOscillator *oscillator, wavtab_t tab, u32 sr);

//static s32 idx_reset(s32 val);

static void oscillator_set_freq(prgmOscillator *oscillator, fix16 freq);

static void oscillator_set_shape(prgmOscillator *oscillator, fract16 wave);

// set modulated phase
//extern void blosc_set_pm(blOsc* osc, fract32 pm);  //IN PROGRESS!

static fract32 oscillator_lookup(prgmOscillator *oscillator);

static fract32 oscillator_next(prgmOscillator *oscillator);

static void oscillator_advance(prgmOscillator *oscillator);

static void calc_frame(void);


//static functions
static inline fract32 freq_to_phase(fix16 f) {
    return add_fr1x32(
                      // int part
                      (fract32)( ((int)(f >> 16) * (int)(WAVE_IPS_NORM) ) ),
                      // fract part
                      mult_fr1x32x32( (f & 0xffff) >> 1, (fract16)(WAVE_IPS_NORM) )
                      );
}


static inline void oscillator_calc_inc(prgmOscillator *oscillator) {
    oscillator->incSlew.x = freq_to_phase(fix16_mul(FIX16_ONE, oscillator->freq));
//    oscillator->inc = freq_to_phase(oscillator->freq);
//    osc->incSlew.x = freq_to_phase(fix16_mul(osc->ratio, osc->hz));
//    oscillator->incSlew.x = freq_to_phase(fix16_mul(ips, oscillator->freq));
}


PrgmOscillatorpointer init(void) {
    return(PrgmOscillatorpointer)malloc(sizeof(prgmOscillator));    
}

    
void init_parameters(prgmOscillator *osc, wavtab_t tab, u32 sr) {
    prgmOscillator *oscillator = (prgmOscillator*)osc;
    
    oscillator->tab = tab;
    slew_init(oscillator->incSlew, 0, 0, 0);
    slew_init(osc->shapeSlew, 0, 0, 0 );
    
    oscillator->frameVal = 0;
    oscillator->phase = 0;
    oscillator->idx = 0;
    oscillator->freq = FIX16_ONE;
    oscillator->wave = 0;
//    oscillator->inc = fix16_mul(oscillator->freq, ips);
    oscillator->amp = INT32_MAX >> 4;
}
//extern void osc_init(osc* osc, wavtab_t tab, u32 sr);


//s32 idx_reset(s32 val) {
//    return val;
//}
/*
 static void set_param_trig(drumsynVoice* vp, s32 val) {
 u8 b = (val > 0);
 env_exp_set_trig( &(vp->envAmp)	, b );
 env_exp_set_trig( &(vp->envFreq)	, b );
 env_exp_set_trig( &(vp->envRq)	, b );
 }
 
 extern void env_exp_set_trig(env_exp* env, u8 gate);
 
 void env_exp_set_trig(env_exp* env, u8 trig) {
 env->trig = trig;
 if(env->state == envStateSus) {
 if(trig) {
 env->stateFP =  &next_sus_trig1;
 } else {
 env->stateFP = &next_sus_trig0;
 }
 }
 }
 */


void oscillator_set_shape(prgmOscillator *oscillator, fract16 wave) {
    oscillator->shapeSlew.x = wave;
}


void oscillator_set_freq(prgmOscillator *oscillator, fix16 freq) {
//    if(freq < OSC_HZ_MIN) freq = OSC_HZ_MIN;
//    if(freq > OSC_HZ_MAX) freq = OSC_HZ_MAX;
    oscillator->freq = freq;
    oscillator_calc_inc(oscillator);
    //    filter_1p_lo_in(&(oscillator->freq));
}
//extern void osc_set_hz(osc* osc, fix16 hz);

/*
fract32 oscillator_lookup(prgmOscillator *oscillator) {
#if 1
    u32 idxA = oscillator->wave >> WAVE_TAB_RSHIFT;
    u32 idxB = idxA + 1;
    
    fract32 mul = (oscillator->wave & WAVE_TAB_MASK) << WAVE_TAB_LSHIFT;
    fract32 mulInv = sub_fr1x32(FR32_MAX, mul);
#else
#endif
    
    return add_fr1x32(
                      mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxA], WAVE_TAB_SIZE, oscillator->idx), mulInv),
                      mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxB], WAVE_TAB_SIZE, oscillator->idx), mul)
                     );
}
*/
/*
fract32 oscillator_lookup(prgmOscillator *oscillator) {     //W SYNC TRIG
#if 1
    u32 idxA = oscillator->wave >> WAVE_TAB_RSHIFT;
    u32 idxB = idxA + 1;
    
    fract32 mul = (oscillator->wave & WAVE_TAB_MASK) << WAVE_TAB_LSHIFT;
    fract32 mulInv = sub_fr1x32(FR32_MAX, mul);
#else
#endif
    
    state = OFF;
    
    if (idx_reset(v) == 0) {
        state = OFF;
        
        return add_fr1x32(
        mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxA], WAVE_TAB_SIZE, oscillator->idx), mulInv),
        mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxB], WAVE_TAB_SIZE, oscillator->idx), mul)
                          );
    }
    
    else if (state == OFF) {
        state = IN;
        oscillator->idx = 0;
        
        return add_fr1x32(
        mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxA], WAVE_TAB_SIZE, oscillator->idx), mulInv),
        mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxB], WAVE_TAB_SIZE, oscillator->idx), mul)
        );
    }
}
*/

static inline fract16 param_unit_to_fr16(ParamValue v) {
    return (fract16)((v & 0xffff) >> 1);
}


static inline fract32 oscillator_lookup(prgmOscillator *oscillator) {
    u32 waveIdxA = oscillator->wave >> (WAVE_SHAPE_IDX_SHIFT);
    u32 waveIdxB = waveIdxA + 1;

    fract16 waveMulB = (oscillator->wave & (WAVE_SHAPE_MASK)) << (WAVE_SHAPE_MUL_SHIFT);
    fract16 waveMulA = sub_fr1x16(0x7fff, waveMulB);
        
    u32 signalIdxA = oscillator->phase >> WAVE_IDX_SHIFT; 
    u32 signalIdxB = (signalIdxA + 1) & WAVE_TAB_SIZE_1;
    fract16 signalMulB = (fract16)((oscillator->phase & (WAVE_IDX_MASK)) >> (WAVE_IDX_MUL_SHIFT));
    fract16 signalMulA = sub_fr1x16(0x7fff, signalMulB);     
    
    return add_fr1x32(
                      // table A
                      mult_fr1x32(
                                  trunc_fr1x32(
                                               add_fr1x32(
                                                          // signal A, scaled
                                                          mult_fr1x32( 
                                                                      trunc_fr1x32( (*(oscillator->tab))[waveIdxA][signalIdxA] ),
                                                                      signalMulA 
                                                                      ),
                                                          // signal B, scaled
                                                          mult_fr1x32( 
                                                                      trunc_fr1x32( (*(oscillator->tab))[waveIdxA][signalIdxB] ),
                                                                      signalMulB
                                                                      )
                                                          )
                                               ),
                                  waveMulA
                                  ),
                      // table B
                      mult_fr1x32(
                                  trunc_fr1x32(
                                               add_fr1x32(
                                                          // signal A, scaled
                                                          mult_fr1x32( 
                                                                      trunc_fr1x32( (*(oscillator->tab))[waveIdxB][signalIdxA] ),
                                                                      signalMulA 
                                                                      ),
                                                          // signal B, scaled
                                                          mult_fr1x32( 
                                                                      trunc_fr1x32( (*(oscillator->tab))[waveIdxB][signalIdxB] ),
                                                                      signalMulB
                                                                      )
                                                          )
                                               ),
                                  waveMulB
                                  )
                      );
}


fract32 oscillator_next(prgmOscillator *oscillator) {
//    slew32_calc(oscillator->incSlew);
    
    slew16_calc (oscillator->shapeSlew);
    slew32_calc (oscillator->incSlew);
    
    oscillator->inc = oscillator->incSlew.y;
    oscillator->wave = oscillator->shapeSlew.y;
    
    oscillator_advance(oscillator);    
    
    return oscillator_lookup(oscillator);
}
/*
// get next value
extern void blosc_get_next(blOsc* osc) {
    // modulated phase
    fix16 phaseMod;
    // phase of inverted signal
    fix16 phaseInv;
    // main signal
    fract32 sig;
    // inverted signal
    fract32 sigInv;
    
    // main phase, with modulation
    phase = fix16_add(osc->phase + osc->modPhase);
    // wrap sum
    while(phase > osc->maxIdx) {
        phase = fix16_sub(phase, osc->maxIdx);
    }  
    
    // main signal
    sig = blosc_lookup(osc, phase);
    //  phase for inverted signal
    phaseInv = fix16_add(phase, osc->invPhase);
    // inverted signal
    sigInv = mult_fr1x32x32(BIT_INVERT_32(blosc_lookup(osc, phase)), osc->invAmp);
    sig = add_fr1x32(sig, sigInv);
    
    // advance main phase
    osc->phase = fix16_add(osc->phase, osc->inc);
    // wrap after advance
    while(osc->phase > osc->maxIdx) {
        osc->phase = fix16_sub(osc->phase, osc->maxIdx);
    }
    // return signal
    return sig;
}
*/


void oscillator_advance(prgmOscillator *oscillator) {
//    oscillator->idx = fix16_add(oscillator->idx, oscillator->inc);
//    while(oscillator->idx > WAVE_TAB_MAX16) {
//    oscillator->idx = fix16_sub(oscillator->idx, WAVE_TAB_MAX16);
//    }

    oscillator->phase = (((int)oscillator->phase) + ((int)(oscillator->inc)) ) & 0x7fffffff;
}


static void calc_frame(void) {    
    oscillator[0]->frameVal = shr_fr1x32(oscillator_next(oscillator[0]), 1);
    oscillator[1]->frameVal = shr_fr1x32(oscillator_next(oscillator[1]), 1);
    oscillator[2]->frameVal = shr_fr1x32(oscillator_next(oscillator[2]), 1);
    oscillator[3]->frameVal = shr_fr1x32(oscillator_next(oscillator[3]), 1);
    
//    oscillator_advance(oscillator[0]);
//    oscillator_advance(oscillator[1]);
//    oscillator_advance(oscillator[2]);
//    oscillator_advance(oscillator[3]);
}


static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}


//external functions
void module_init(void) {
    u8 i;
    data = (prgmData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "aleph-prgm");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;
    
    sr = SAMPLERATE;
//    ips = fix16_from_float((f32)WAVE_TAB_SIZE / (f32)sr);

    for(i=0; i<N_OSCILLATORS; i++) {
        oscillator[i] = init();
        init_parameters(oscillator[i], &wavtab, SAMPLERATE);
    }
    
    ioAmp0 = FR32_MAX;
    ioAmp1 = FR32_MAX;
    ioAmp2 = FR32_MAX;
    ioAmp3 = FR32_MAX;

    param_setup(eParamFreq0, 220 << 16);
    param_setup(eParamFreq1, 220 << 16);
    param_setup(eParamFreq2, 220 << 16);
    param_setup(eParamFreq3, 220 << 16);

    param_setup(eParamWave0, 0);
    param_setup(eParamWave1, 0);
    param_setup(eParamWave2, 0);
    param_setup(eParamWave3, 0);
    
//    param_setup(eParamIdxReset, 0);
        
    param_setup(eParamAmp0, PARAM_AMP_6);
    param_setup(eParamAmp1, PARAM_AMP_6);
    param_setup(eParamAmp2, PARAM_AMP_6);
    param_setup(eParamAmp3, PARAM_AMP_6);
}


void module_deinit(void) {}


extern u32 module_get_num_params(void) {
  return eParamNumParams;
}


void module_process_frame(void) {
  calc_frame();
//remove in?!
    out[0] = add_fr1x32(oscillator[0]->frameVal, mult_fr1x32x32(in[0], ioAmp0));
    out[1] = add_fr1x32(oscillator[1]->frameVal, mult_fr1x32x32(in[1], ioAmp1));
    out[2] = add_fr1x32(oscillator[2]->frameVal, mult_fr1x32x32(in[2], ioAmp2));
    out[3] = add_fr1x32(oscillator[3]->frameVal, mult_fr1x32x32(in[3], ioAmp3));
}


void module_set_param(u32 idx, ParamValue v) {
    switch(idx) {
        case eParamFreq0:
            oscillator_set_freq(oscillator[0], v);
            break;
        case eParamFreq1:
            oscillator_set_freq(oscillator[1], v);
            break;
        case eParamFreq2:
            oscillator_set_freq(oscillator[2], v);
            break;
        case eParamFreq3:
            oscillator_set_freq(oscillator[3], v);
            break;
            
        case eParamWave0:
            oscillator_set_shape(oscillator[0], param_unit_to_fr16(v));
            break;
        case eParamWave1:
            oscillator_set_shape(oscillator[1], param_unit_to_fr16(v));
            break;
        case eParamWave2:
            oscillator_set_shape(oscillator[2], param_unit_to_fr16(v));
            break;
        case eParamWave3:
            oscillator_set_shape(oscillator[3], param_unit_to_fr16(v));
            break;

//        case eParamIdxReset:
//            idx_reset(v);
//            break;

/*
        case eParamIdxMod0:
            //function(&(oscillator[0]->idxMod), v);
            break;
        case eParamIdxMod1:
            //function(&(oscillator[0]->idxMod), v);
            break;
        case eParamIdxMod2:
            //function(&(oscillator[0]->idxMod), v);
            break;
        case eParamIdxMod3:
            //function(&(oscillator[0]->idxMod), v);
            break;
*/

        case eParamAmp0:
            filter_1p_lo_in(&(oscillator[0]->amp), v);
            break;
        case eParamAmp1:
            filter_1p_lo_in(&(oscillator[1]->amp), v);
            break;
        case eParamAmp2:
            filter_1p_lo_in(&(oscillator[2]->amp), v);
            break;
        case eParamAmp3:
            filter_1p_lo_in(&(oscillator[3]->amp), v);
            break;

/*
        case eParamFreq0Slew:
            filter_1p_lo_set_slew(freq0Lp, v);
            break;
        case eParamFreq1Slew:
            filter_1p_lo_set_slew(freq1Lp, v);
            break;
        case eParamFreq2Slew:
            filter_1p_lo_set_slew(freq2Lp, v);
            break;
        case eParamFreq3Slew:
            filter_1p_lo_set_slew(freq3Lp, v);
            break;

        case eParamWave0Slew:
            filter_1p_lo_set_slew(wave0Lp, v);
            break;
        case eParamWave1Slew:
            filter_1p_lo_set_slew(wave1Lp, v);
            break;
        case eParamWave2Slew:
            filter_1p_lo_set_slew(wave2Lp, v);
            break;
        case eParamWave3Slew:
            filter_1p_lo_set_slew(wave3Lp, v);
            break;

        case eParamAmp0Slew:
            filter_1p_lo_set_slew(amp0Lp, v);
            break;
        case eParamAmp1Slew:
            filter_1p_lo_set_slew(amp1Lp, v);
            break;
        case eParamAmp2Slew:
            filter_1p_lo_set_slew(amp2Lp, v);
            break;
        case eParamAmp3Slew:
            filter_1p_lo_set_slew(amp3Lp, v);
            break;
*/

        case eParamIoAmp0:
            ioAmp0 = (v);
            break;
        case eParamIoAmp1:
            ioAmp1 = (v);
            break;
        case eParamIoAmp2:
            ioAmp2 = (v);
            break;
        case eParamIoAmp3:
            ioAmp3 = (v);
            break;
        default:
            break;
    }
}
