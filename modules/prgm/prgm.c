//prgm.c
//aleph-bfin

#include "prgm.h"

//data types
typedef struct _prgmData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    Wavtabs mWavtabs;
} prgmData;

ModuleData *gModuleData;

prgmOscillator *oscillator[N_OSCILLATORS];

static prgmData *data;

//static variables
static fract32 wavtabA[WAVE_SHAPE_NUM][WAVE_TAB_SIZE] = {
#include "wavtab512_wf0.c"
};

static fract32 wavtabB[WAVE_SHAPE_NUM][WAVE_TAB_SIZE] = {
#include "wavtab512_wf1.c"
};

static fract32 wavtab0[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];
static fract32 wavtab1[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];
static fract32 wavtab2[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];
static fract32 wavtab3[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

static void fill_wavetable(fract32 to[WAVE_SHAPE_NUM][WAVE_TAB_SIZE], fract32 from[WAVE_SHAPE_NUM][WAVE_TAB_SIZE]) {
    int i;
    
    i = 0;
    while (i < WAVE_TAB_SIZE) {
        to[0][i] = from[0][i];
        i++;
    }
    i = 0;
    while (i < WAVE_TAB_SIZE) {
        to[1][i] = from[1][i];
        i++;
    }
}

static void oscillator_set_wave0(prgmOscillator *oscillator, ParamValue v) {
    if (v < 1) {
        fill_wavetable(wavtab0, wavtabA);
        oscillator->tab = &wavtab0;
    }
    else if (v > 1) { 
        fill_wavetable(wavtab0, wavtabB);
        oscillator->tab = &wavtab0;
    }
    else
        ;
}

static void oscillator_set_wave1(prgmOscillator *oscillator, ParamValue v) {
    if (v < 1) {
        fill_wavetable(wavtab1, wavtabA);
        oscillator->tab = &wavtab1;
    }
    else if (v > 1) { 
        fill_wavetable(wavtab1, wavtabB);
        oscillator->tab = &wavtab1;
    }
    else
        ;
}

static void oscillator_set_wave2(prgmOscillator *oscillator, ParamValue v) {
    if (v < 1) {
        fill_wavetable(wavtab2, wavtabA);
        oscillator->tab = &wavtab2;
    }
    else if (v > 1) { 
        fill_wavetable(wavtab2, wavtabB);
        oscillator->tab = &wavtab2;
    }
    else
        ;
}

static void oscillator_set_wave3(prgmOscillator *oscillator, ParamValue v) {
    if (v < 1) {
        fill_wavetable(wavtab3, wavtabA);
        oscillator->tab = &wavtab3;
    }
    else if (v > 1) { 
        fill_wavetable(wavtab3, wavtabB);
        oscillator->tab = &wavtab3;
    }
    else
        ;
}

static u32 sr;


//static function declarations
static inline fract32 freq_to_phase(fix16 freq);

static inline void oscillator_calc_inc(prgmOscillator *oscillator);

//init
PrgmOscillatorpointer init(void);
static void init_parameters(prgmOscillator *oscillator, WavtabData tab, u32 sr);

static void oscillator_set_f(prgmOscillator *oscillator, fix16 f);
static void oscillator_set_ff(prgmOscillator *oscillator, fix16 ff);
static void oscillator_set_ffamount(prgmOscillator *oscillator, fix16 ffAmount);

static void oscillator_set_shape(prgmOscillator *oscillator, fract16 wave);

static fract32 oscillator_lookup(prgmOscillator *oscillator);

static fract32 oscillator_next(prgmOscillator *oscillator);

static void oscillator_advance(prgmOscillator *oscillator);

static void calc_frame(void);

//sync trig
static void oscillator_sync_in(ParamValue v);
static void prgm_sync_trig(void);
static void oscillator_set_trippoint(prgmOscillator *oscillator, fract32 trip);


//static functions
fract32 freq_to_phase(fix16 freq) {
    return add_fr1x32(
                      // int part
                      (fract32)( ((int)(freq >> 16) * (int)(WAVE_IPS_NORM) ) ),
                      // fract part
                      mult_fr1x32x32( (freq & 0xffff) >> 1, (fract16)(WAVE_IPS_NORM) )
                      );
}


void oscillator_calc_inc(prgmOscillator *oscillator) {
    oscillator->incSlew.x = freq_to_phase(fix16_mul(FIX16_ONE, (fix16_add(oscillator->f, oscillator->ff))));
}


PrgmOscillatorpointer init(void) {
    return(PrgmOscillatorpointer)malloc(sizeof(prgmOscillator));    
}


void init_parameters(prgmOscillator *osc, WavtabData tab, u32 sr) {
    prgmOscillator *oscillator = (prgmOscillator*)osc;
    
    oscillator->tab = tab;
    slew_init(oscillator->incSlew, 0, 0, 0);
    slew_init(oscillator->ffSlew, 0, 0, 0);
    slew_init(oscillator->shapeSlew, 0, 0, 0);
    
    oscillator->frameVal = 0;
    oscillator->phase = 0;
    oscillator->trip = 0;
    oscillator->tripPoint = &(oscillator->phase);
    oscillator->f = FIX16_ONE;
    oscillator->ff = 0;
    oscillator->ffAmount = 0;
    oscillator->wave = 0;
    oscillator->amp = INT32_MAX >> 4;
}


void oscillator_set_shape(prgmOscillator *oscillator, fract16 wave) {
    oscillator->shapeSlew.x = wave;
}


void oscillator_set_f(prgmOscillator *oscillator, fix16 f) {
    oscillator->f = f;
    oscillator_calc_inc(oscillator);
}


//017 TEST ffslew
void oscillator_set_ff(prgmOscillator *oscillator, fix16 ff) {
    oscillator->ffSlew.x = fix16_mul(ff, oscillator->ffAmount);
    oscillator_calc_inc(oscillator);
    
//FM: cos(angle += (incr + change))
}


//017 TEST 1v/oct scaling
void oscillator_set_ffamount(prgmOscillator *oscillator, fix16 ffAmount) {
    oscillator->ffAmount = (ffAmount * 16) / 15;  //scaled to 1v/oct
}


void oscillator_sync_in(ParamValue v) {
    synctrig = v;
}


void prgm_sync_trig(void) {
    if(synctrig == 0)
        state = OFF;
    
    else if(state == OFF) {
        state = ON;
    
        *oscillator[0]->tripPoint &= 0x80000000;
        *oscillator[0]->tripPoint = add_fr1x32((int)oscillator[0]->trip, 0x80000000) & 0x7fffffff;
        
        *oscillator[1]->tripPoint &= 0x80000000;
        *oscillator[1]->tripPoint = add_fr1x32((int)oscillator[1]->trip, 0x80000000) & 0x7fffffff;
        
        *oscillator[2]->tripPoint &= 0x80000000;
        *oscillator[2]->tripPoint = add_fr1x32((int)oscillator[2]->trip, 0x80000000) & 0x7fffffff;
        
        *oscillator[3]->tripPoint &= 0x80000000;
        *oscillator[3]->tripPoint = add_fr1x32((int)oscillator[3]->trip, 0x80000000) & 0x7fffffff;
    }
    
    else if(state == ON)
        ;
}


void oscillator_set_trippoint(prgmOscillator *oscillator, fract32 trip) {
    oscillator->trip = trip;
}


static inline fract16 param_unit_to_fr16(ParamValue v) {
    return (fract16)((v & 0xffff) >> 1);
}


static inline fract32 param_unit_to_fr32(ParamValue v) {
    return fr16_to_fr32((fract16)((v & 0xffff) >> 1));
}


static inline fract32 oscillator_lookup(prgmOscillator *oscillator) {

    u32 waveIdxA = oscillator->wave >> (WAVE_SHAPE_IDX_SHIFT);
    u32 waveIdxB = waveIdxA + 1;

    fract16 waveMulB = oscillator->wave;
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


//017 TEST ffslew
fract32 oscillator_next(prgmOscillator *oscillator) {                           
    slew16_calc (oscillator->shapeSlew);
    slew32_calc (oscillator->ffSlew);
    slew32_calc (oscillator->incSlew);
    
    oscillator->inc = oscillator->incSlew.y;
    oscillator->ff = oscillator->ffSlew.y;
    oscillator->wave = oscillator->shapeSlew.y;

    oscillator_advance(oscillator);

    prgm_sync_trig();

    return oscillator_lookup(oscillator);
}


void oscillator_advance(prgmOscillator *oscillator) {
    oscillator->phase = (((int)oscillator->phase) + ((int)(oscillator->inc))) & 0x7fffffff;
}


//017 TEST...
/*
static void oscillator_amp (void) {
buffer
delay
feedback
phase invert
*/


static void calc_frame(void) {
    oscillator[0]->frameVal = shr_fr1x32(oscillator_next(oscillator[0]), 1);
    oscillator[1]->frameVal = shr_fr1x32(oscillator_next(oscillator[1]), 1);
    oscillator[2]->frameVal = shr_fr1x32(oscillator_next(oscillator[2]), 1);
    oscillator[3]->frameVal = shr_fr1x32(oscillator_next(oscillator[3]), 1);

//017 TEST...
//    mix_adc();
//    out[0] = add_fr1x32((shr_fr1x32(oscillator_next(oscillator[0]), 1)), 0x7fffffff);
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
    gModuleData->wavtabs = data->mWavtabs;
    data->mWavtabs.wavtab0 = &wavtab0;
    data->mWavtabs.wavtab1 = &wavtab1;
    data->mWavtabs.wavtab2 = &wavtab2;
    data->mWavtabs.wavtab3 = &wavtab3;
    
    sr = SAMPLERATE;

    for(i=0; i<N_OSCILLATORS; i++) { oscillator[i] = init(); }
    
    init_parameters(oscillator[0], &wavtab0, SAMPLERATE);
    init_parameters(oscillator[1], &wavtab1, SAMPLERATE);
    init_parameters(oscillator[2], &wavtab2, SAMPLERATE);
    init_parameters(oscillator[3], &wavtab3, SAMPLERATE);
    
    param_setup(eParamFreq0, 220 << 16);
    param_setup(eParamFreq1, 220 << 16);
    param_setup(eParamFreq2, 220 << 16);
    param_setup(eParamFreq3, 220 << 16);

    param_setup(eParamFreqFine0, 0);
    param_setup(eParamFreqFine1, 0);
    param_setup(eParamFreqFine2, 0);
    param_setup(eParamFreqFine3, 0);
    
    param_setup(eParamFFAmount0, 0);
    param_setup(eParamFFAmount1, 0);
    param_setup(eParamFFAmount2, 0);
    param_setup(eParamFFAmount3, 0);
    
    param_setup(eParamTab0, 0);
    param_setup(eParamTab1, 0);
    param_setup(eParamTab2, 0);
    param_setup(eParamTab3, 0);

    param_setup(eParamWave0, 0);
    param_setup(eParamWave1, 0);
    param_setup(eParamWave2, 0);
    param_setup(eParamWave3, 0);
    
    param_setup(eParamSyncTrig, 0);
    
    param_setup(eParamTripPoint0, 0);
    param_setup(eParamTripPoint1, 0);
    param_setup(eParamTripPoint2, 0);
    param_setup(eParamTripPoint3, 0);
            
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
    
    out[0] = add_fr1x32(oscillator[0]->frameVal, 0x7fffffff);
    out[1] = add_fr1x32(oscillator[1]->frameVal, 0x7fffffff);
    out[2] = add_fr1x32(oscillator[2]->frameVal, 0x7fffffff);
    out[3] = add_fr1x32(oscillator[3]->frameVal, 0x7fffffff);
}


void module_set_param(u32 idx, ParamValue v) {
    switch(idx) {
        case eParamFreq0:
            oscillator_set_f(oscillator[0], v);
            break;
        case eParamFreq1:
            oscillator_set_f(oscillator[1], v);
            break;
        case eParamFreq2:
            oscillator_set_f(oscillator[2], v);
            break;
        case eParamFreq3:
            oscillator_set_f(oscillator[3], v);
            break;
            
        case eParamFreqFine0:
            oscillator_set_ff(oscillator[0], v);
            break;
        case eParamFreqFine1:
            oscillator_set_ff(oscillator[1], v);
            break;
        case eParamFreqFine2:
            oscillator_set_ff(oscillator[2], v);
            break;
        case eParamFreqFine3:
            oscillator_set_ff(oscillator[3], v);
            break;

        case eParamFFAmount0:
            oscillator_set_ffamount(oscillator[0], v);
            break;
        case eParamFFAmount1:
            oscillator_set_ffamount(oscillator[1], v);
            break;
        case eParamFFAmount2:
            oscillator_set_ffamount(oscillator[2], v);
            break;
        case eParamFFAmount3:
            oscillator_set_ffamount(oscillator[3], v);
            break;
                        
        case eParamTab0: //RENAME TO eParamWave...
            oscillator_set_wave0(oscillator[0], v);
            break;
        case eParamTab1:
            oscillator_set_wave1(oscillator[1], v);
            break;
        case eParamTab2:
            oscillator_set_wave2(oscillator[2], v);
            break;
        case eParamTab3:
            oscillator_set_wave3(oscillator[3], v);
            break; 

        case eParamWave0: //RENAME TO eParamBlend...
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

        case eParamSyncTrig:
            oscillator_sync_in(v);
            break;

        case eParamTripPoint0:
            oscillator_set_trippoint(oscillator[0], param_unit_to_fr32(v));
            break;
        case eParamTripPoint1:
            oscillator_set_trippoint(oscillator[1], param_unit_to_fr32(v));
            break;
        case eParamTripPoint2:
            oscillator_set_trippoint(oscillator[2], param_unit_to_fr32(v));
            break;
        case eParamTripPoint3:
            oscillator_set_trippoint(oscillator[3], param_unit_to_fr32(v));
            break;
            
        case eParamAmp0:
            oscillator[0]->amp = (v);
            break;
        case eParamAmp1:
            oscillator[1]->amp = (v);
            break;
        case eParamAmp2:
            oscillator[2]->amp = (v);
            break;
        case eParamAmp3:
            oscillator[3]->amp = (v);
            break;

        default:
            break;
    }
}
