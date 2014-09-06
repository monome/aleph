//prgm.c
//aleph-bfin

#include "prgm.h"

//data types
typedef struct _prgmData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    BufferData mBufferData[WAVE_BUF_SIZE];
    BufferTap mBufferTap[WAVE_BUF_SIZE];
    fract32 wave[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];
} prgmData;

ModuleData *gModuleData;

//pointer to SDRAM
prgmData *data;

prgmOscillator *oscillator[N_OSCILLATORS];
prgmCvChannel *cvchannel[N_CVOUTPUTS];

u8 cvChan = 0;

static u32 sr;

//static function declarations
static inline fract32 freq_to_phase(fix16 freq);

static inline void oscillator_calc_inc(prgmOscillator *oscillator);
static inline void cv_calc_inc(prgmCvChannel *cvchannel);

PrgmOscillatorpointer init(void);
PrgmCvChannelpointer init_channel(void);
static void init_parameters(prgmOscillator *oscillator, wave tab, u32 sr);
static void init_cv_parameters(prgmCvChannel *cvchannel);

static void oscillator_set_f(prgmOscillator *oscillator, fix16 f);
static void oscillator_set_t(prgmOscillator *oscillator, fix16 t);

static void oscillator_set_wave(prgmOscillator *oscillator, s32 dummy);
static void oscillator_set_shape(prgmOscillator *oscillator, fract16 wave);

static fract32 oscillator_lookup(prgmOscillator *oscillator);

static fract32 oscillator_next(prgmOscillator *oscillator);

static void oscillator_advance(prgmOscillator *oscillator);

static void calc_frame(void);

static void cv_set_f(prgmCvChannel *cvchannel, fract32 f);
static void cv_set_t(prgmCvChannel *cvchannel, fract32 t);

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
    oscillator->incSlew.x = freq_to_phase(fix16_mul(FIX16_ONE, (fix16_mul(oscillator->f, oscillator->t))));
}


void cv_calc_inc(prgmCvChannel *cvchannel) {
    filter_1p_lo_in(&(cvchannel->cvSlew), fix16_mul(FIX16_ONE, (fix16_mul(cvchannel->f, cvchannel->t))));
    
    if(filter_1p_sync(&(cvchannel->cvSlew))) {
        ;;
    } else {
        cvchannel->cvVal = filter_1p_lo_next(&(cvchannel->cvSlew));
        cv_update(cvChan, cvchannel->cvVal);
    }
}


PrgmOscillatorpointer init(void) {
    return(PrgmOscillatorpointer)malloc(sizeof(prgmOscillator));
}


PrgmCvChannelpointer init_channel(void) {
    return(PrgmCvChannelpointer)malloc(sizeof(prgmCvChannel));
}


void init_parameters(prgmOscillator *osc, wave tab, u32 sr) {
    prgmOscillator *oscillator = (prgmOscillator*)osc;
    
    oscillator->tab = tab;
    slew_init(oscillator->incSlew, 0, 0, 0);
    slew_init(oscillator->shapeSlew, 0, 0, 0);
    
    oscillator->frameVal = 0;
    oscillator->phase = 0;
    oscillator->trip = 0;
    oscillator->tripPoint = &(oscillator->phase);
    oscillator->f = 0;
    oscillator->t = 0;
    oscillator->wave = 0;
    oscillator->amp = INT32_MAX >> 4; //TEST!!!
}


void init_cv_parameters(prgmCvChannel *cv) {
    prgmCvChannel *cvchannel = (prgmCvChannel*)cv;

    cvchannel->cvVal = 0;
    cvchannel->f = 0;
    cvchannel->t = 0;
    filter_1p_lo_in(&(cvchannel->cvSlew), 0xf);
}


void init_buffer(BufferData *buf, fract32 *data, u32 count) {
    buf->bufdata = data;
    buf->bufcount = count;
}


void init_buffer_tap(BufferTap *tap, BufferData *buf) {
    tap->buf = buf;
    tap->bufpos = 0;
}


void buffer_tap_set_pos(BufferTap *tap, u32 pos) {
    tap->bufpos = pos;
}


void buffer_tap_write(BufferTap *tap, fract32 data) {
    tap->buf->bufdata[tap->bufpos] = data;
}


fract32 buffer_tap_read(BufferTap *tap) {
//    s32 tmp = tap->buf->bufdata[tap->bufpos];
//    fract16 tmp16 = (fract16)((tmp) & 0xffff) >> 1;
//    fract32 tmp32 = fr16_to_fr32((fract16)((tmp16 & 0xffff) >> 1));
//    return tmp32;
    return tap->buf->bufdata[tap->bufpos];
}


void buffer_tap_next(BufferTap *tap) {
    tap->bufpos++;
    if (tap->bufpos >= tap->buf->bufcount) {
        tap->bufpos = 0; //0x80000000 & 0x7fffffff;
    }
}


void oscillator_set_wave(prgmOscillator *oscillator, s32 dummy) {
    switch(dummy) {
        case 0:
            module_load_wavetable(data->mBufferData, data->mBufferTap);
//            oscillator->tab = &data->wave;
            break;
/*
        case 1:
            oscillator->tab = data->wave1;
            break;
        case 2:
            oscillator->tab = data->wave2;
            break;
        case 3:
            oscillator->tab = data->wave3;
            break;
*/
        default:
            break;
    }
}



void oscillator_set_shape(prgmOscillator *oscillator, fract16 wave) {
    oscillator->shapeSlew.x = wave;
}


void oscillator_set_f(prgmOscillator *oscillator, fix16 f) {
    oscillator->f = f;
    oscillator_calc_inc(oscillator);
}


void oscillator_set_t(prgmOscillator *oscillator, fix16 t) {
    oscillator->t = t;
    oscillator_calc_inc(oscillator);
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


void cv_set_f(prgmCvChannel *cvchannel, fix16 f) {
    cvchannel->f = f;
    cv_calc_inc(cvchannel);
}


void cv_set_t(prgmCvChannel *cvchannel, fix16 t) {
    cvchannel->t = t;
    cv_calc_inc(cvchannel);
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
    gModuleData->bufferData = data->mBufferData;
    gModuleData->bufferTap = data->mBufferTap;
    
    sr = SAMPLERATE;

    init_buffer(data->mBufferData, data->mBufferData[i].bufdata, WAVE_BUF_SIZE);
    init_buffer_tap(data->mBufferTap, data->mBufferData);
    
    data->mBufferData[i].bufdata = 0;
    
    buffer_tap_set_pos(data->mBufferTap, 0);
        
    for(i=0; i<N_OSCILLATORS; i++) { oscillator[i] = init(); }
    for(i=0; i<N_CVOUTPUTS; i++) { cvchannel[i] = init_channel(); }
    
    init_parameters(oscillator[0], &data->wave, SAMPLERATE);
    init_parameters(oscillator[1], &data->wave, SAMPLERATE);
    init_parameters(oscillator[2], &data->wave, SAMPLERATE);
    init_parameters(oscillator[3], &data->wave, SAMPLERATE);
    
    init_cv_parameters(cvchannel[0]);
    init_cv_parameters(cvchannel[1]);
    init_cv_parameters(cvchannel[2]);
    init_cv_parameters(cvchannel[3]);
    
    param_setup(eParamFreq0, 152 << 16);
    param_setup(eParamFreq1, 152 << 16);
    param_setup(eParamFreq2, 152 << 16);
    param_setup(eParamFreq3, 152 << 16);

    param_setup(eParamTranspose0, 1 << 16);
    param_setup(eParamTranspose1, 1 << 16);
    param_setup(eParamTranspose2, 1 << 16);
    param_setup(eParamTranspose3, 1 << 16);
        
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
    
    param_setup(eParamCVfreq0, PARAM_AMP_6);
    param_setup(eParamCVfreq1, PARAM_AMP_6);
    param_setup(eParamCVfreq2, PARAM_AMP_6);
    param_setup(eParamCVfreq3, PARAM_AMP_6);

    param_setup(eParamCVtranspose0, PARAM_AMP_6);
    param_setup(eParamCVtranspose1, PARAM_AMP_6);
    param_setup(eParamCVtranspose2, PARAM_AMP_6);
    param_setup(eParamCVtranspose3, PARAM_AMP_6);
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
            
        case eParamTranspose0:
            oscillator_set_t(oscillator[0], v);
            break;
        case eParamTranspose1:
            oscillator_set_t(oscillator[1], v);
            break;
        case eParamTranspose2:
            oscillator_set_t(oscillator[2], v);
            break;
        case eParamTranspose3:
            oscillator_set_t(oscillator[3], v);
            break;
            
        case eParamTab0: //RENAME TO eParamWave...
            oscillator_set_wave(oscillator[0], v);
            break;
        case eParamTab1:
            oscillator_set_wave(oscillator[1], v);
            break;
        case eParamTab2:
            oscillator_set_wave(oscillator[2], v);
            break;
        case eParamTab3:
            oscillator_set_wave(oscillator[3], v);
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
            
        case eParamCVfreq0:
            cv_set_f(cvchannel[0], v);
            cvChan = 0;
            break;
        case eParamCVfreq1:
            cv_set_f(cvchannel[1], v);
            cvChan = 1;
            break;
        case eParamCVfreq2:
            cv_set_f(cvchannel[2], v);
            cvChan = 2;
            break;
        case eParamCVfreq3:
            cv_set_f(cvchannel[3], v);
            cvChan = 3;
            break;
            
        case eParamCVtranspose0:
            cv_set_t(cvchannel[0], v);
            cvChan = 0;
            break;
        case eParamCVtranspose1:
            cv_set_t(cvchannel[1], v);
            cvChan = 1;
            break;
        case eParamCVtranspose2:
            cv_set_t(cvchannel[2], v);
            cvChan = 2;
            break;
        case eParamCVtranspose3:
            cv_set_t(cvchannel[3], v);
            cvChan = 3;
            break;

        default:
            break;
    }
}


void module_set_wave(fract32 frame) {
    buffer_tap_write(data->mBufferTap, frame);
    buffer_tap_next(data->mBufferTap);
}


void module_load_wavetable(BufferData *buf, BufferTap *tap) {
    buffer_tap_set_pos(data->mBufferTap, 0);
    
    if (data->mBufferTap->bufpos < 512) {
        data->wave[0][data->mBufferTap->bufpos] = buffer_tap_read(data->mBufferTap);
        buffer_tap_next(data->mBufferTap);
    }
    else if (data->mBufferTap->bufpos < 1024) {
        data->wave[1][data->mBufferTap->bufpos] = buffer_tap_read(data->mBufferTap);
        buffer_tap_next(data->mBufferTap);
    }
}
