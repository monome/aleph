//prgm.c
//aleph-bfin

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

//fixed variables
static u32 sr;


//static function declaration
static inline fract32 freq_to_phase(fix16 freq);

static inline void oscillator_calc_inc(prgmOscillator *oscillator);

//init
PrgmOscillatorpointer init(void);
static void init_parameters(prgmOscillator *oscillator, wavtab_t tab, u32 sr);

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
static void oscillator_set_trippoint(prgmOscillator *oscillator, fix16 tripPoint);


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

    
void init_parameters(prgmOscillator *osc, wavtab_t tab, u32 sr) {
    prgmOscillator *oscillator = (prgmOscillator*)osc;
    
    oscillator->tab = tab;
    slew_init(oscillator->incSlew, 0, 0, 0);
    slew_init(oscillator->shapeSlew, 0, 0, 0);
    
    oscillator->frameVal = 0;
    oscillator->phase = 0;
    oscillator->tripPoint = 0;
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


void oscillator_set_ff(prgmOscillator *oscillator, fix16 ff) {
    oscillator->ff = fix16_mul(ff, oscillator->ffAmount);
    oscillator_calc_inc(oscillator);
}


void oscillator_set_ffamount(prgmOscillator *oscillator, fix16 ffAmount) {
    oscillator->ffAmount = ffAmount;
}


void oscillator_sync_in(ParamValue v) {
    synctrig = v;
}


void prgm_sync_trig(void) {
    if(synctrig == 0)
        state = OFF;
    
    else if(state == OFF) {
        state = ON;
        
        oscillator[0]->phase = oscillator[0]->tripPoint;
        oscillator[1]->phase = oscillator[1]->tripPoint;
        oscillator[2]->phase = oscillator[2]->tripPoint;
        oscillator[3]->phase = oscillator[3]->tripPoint;
    }
    
    else if(state == ON)
        ;
}


void oscillator_set_trippoint(prgmOscillator *oscillator, fix16 tripPoint) {
    oscillator->tripPoint = oscillator->phase + fix16_mul(oscillator->inc, tripPoint);
}
                           

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
    slew16_calc (oscillator->shapeSlew);
    slew32_calc (oscillator->incSlew);
    
    oscillator->inc = oscillator->incSlew.y;
    oscillator->wave = oscillator->shapeSlew.y;

    oscillator_advance(oscillator);

    prgm_sync_trig();
            
    return oscillator_lookup(oscillator);
}


void oscillator_advance(prgmOscillator *oscillator) {
    oscillator->phase = (((int)oscillator->phase) + ((int)(oscillator->inc))) & 0x7fffffff;
}


static void calc_frame(void) {
    oscillator[0]->frameVal = shr_fr1x32(oscillator_next(oscillator[0]), 1);
    oscillator[1]->frameVal = shr_fr1x32(oscillator_next(oscillator[1]), 1);
    oscillator[2]->frameVal = shr_fr1x32(oscillator_next(oscillator[2]), 1);
    oscillator[3]->frameVal = shr_fr1x32(oscillator_next(oscillator[3]), 1);
    
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
    
    sr = SAMPLERATE;

    for(i=0; i<N_OSCILLATORS; i++) {
        oscillator[i] = init();
        init_parameters(oscillator[i], &wavtab, SAMPLERATE);
    }
    
    param_setup(eParamFreq0, 220 << 16);
    param_setup(eParamFreq1, 220 << 16);
    param_setup(eParamFreq2, 220 << 16);
    param_setup(eParamFreq3, 220 << 16);

    param_setup(eParamFreqFine0, 0 << 16);
    param_setup(eParamFreqFine1, 0 << 16);
    param_setup(eParamFreqFine2, 0 << 16);
    param_setup(eParamFreqFine3, 0 << 16);
    
    param_setup(eParamFFAmount0, 0);
    param_setup(eParamFFAmount1, 0);
    param_setup(eParamFFAmount2, 0);
    param_setup(eParamFFAmount3, 0);

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

        case eParamSyncTrig:
            oscillator_sync_in(v);
            break;

        case eParamTripPoint0:
            oscillator_set_trippoint(oscillator[0], v);
            break;
        case eParamTripPoint1:
            oscillator_set_trippoint(oscillator[1], v);
            break;
        case eParamTripPoint2:
            oscillator_set_trippoint(oscillator[2], v);
            break;
        case eParamTripPoint3:
            oscillator_set_trippoint(oscillator[3], v);
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
