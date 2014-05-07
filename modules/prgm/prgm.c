//prgm.c
//aleph-bfin

//standard libraries
#include <math.h>
#include <stdlib.h>
#include <string.h>

//aleph-common
#include "fix.h"

//audiolib
#include "filter_1p.h"
#include "table.h"
#include "conversion.h"

//bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "types.h"
#include "params.h"


//symbolic constants, macro substitutions
#define WAVE_TAB_NUM 5
#define WAVE_TAB_SIZE 1024
#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)
#define WAVE_TAB_MAX16 (WAVE_TAB_SIZE * FIX16_ONE - 1)
#define WAVE_TAB_RSHIFT 29
#define WAVE_TAB_MASK 0x1fffffff
#define WAVE_TAB_LSHIFT 2


//data types
typedef struct _prgmData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} prgmData;

ModuleData *gModuleData; // module data

static prgmData *data;

static u32      sr;     //sample rate
static fix16    ips;    //index change per sample

static fix16    idx0;   //current phase (fractional idx)
static fix16    idx1;
static fix16    idx2;
static fix16    idx3;

static fix16    inc0;   //idx change at current frequency
static fix16    inc1;
static fix16    inc2;
static fix16    inc3;

static fract32  osc0;   //oscillator output
static fract32  osc1;
static fract32  osc2;
static fract32  osc3;

static fract32  frameVal;   //output value
static fract32  tmp0;
static fract32  tmp1;
static fract32  tmp2;
static fract32  tmp3;

static fix16    freq0;  //base frequency
static fix16    freq1;
static fix16    freq2;
static fix16    freq3;

static fract32  wave0;  //waveshape
static fract32  wave1;
static fract32  wave2;
static fract32  wave3;

static fract32  amp0;   //amplitude
static fract32  amp1;
static fract32  amp2;
static fract32  amp3;

static filter_1p_lo *freq0Lp;   //1plp slew
static filter_1p_lo *freq1Lp;
static filter_1p_lo *freq2Lp;
static filter_1p_lo *freq3Lp;

static filter_1p_lo *wave0Lp;   //1plp slew
static filter_1p_lo *wave1Lp;
static filter_1p_lo *wave2Lp;
static filter_1p_lo *wave3Lp;

static filter_1p_lo *amp0Lp;    //1plp slew
static filter_1p_lo *amp1Lp;
static filter_1p_lo *amp2Lp;
static filter_1p_lo *amp3Lp;

static fract32 ioAmp0;
static fract32 ioAmp1;
static fract32 ioAmp2;
static fract32 ioAmp3;


//static variables
//pointer to local module data, initialize/v at top of SDRAM

static const fract32 wavtab[WAVE_TAB_NUM][WAVE_TAB_SIZE] = { 
#include "wavtab_data_inc.c" 
};



//static function declaration
static inline void set_freq0(fix16 freq);
static inline void set_freq1(fix16 freq);
static inline void set_freq2(fix16 freq);
static inline void set_freq3(fix16 freq);
static inline fract32 lookup_wave(const fix16 idx, const fract32 wave);
static void calc_frame(void);


//static functions
static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}


static inline void set_freq0(fix16 freq) {  
    if(freq < OSC_FREQ_MIN) freq = OSC_FREQ_MIN;
    if(freq > OSC_FREQ_MAX) freq = OSC_FREQ_MAX;
    filter_1p_lo_in(freq0Lp, freq);
    freq0 = freq;
//    freq = &trash;
}


static inline void set_freq1(fix16 freq) {
    if(freq < OSC_FREQ_MIN) freq = OSC_FREQ_MIN;
    if(freq > OSC_FREQ_MAX) freq = OSC_FREQ_MAX;
    filter_1p_lo_in(freq1Lp, freq);
    freq1 = freq;
//    freq = &trash;
}


static inline void set_freq2(fix16 freq) {
    if(freq < OSC_FREQ_MIN) freq = OSC_FREQ_MIN;
    if(freq > OSC_FREQ_MAX) freq = OSC_FREQ_MAX;
    filter_1p_lo_in(freq2Lp, freq);
    freq2 = freq;
//    freq = &trash;
}


static inline void set_freq3(fix16 freq) {
    if(freq < OSC_FREQ_MIN) freq = OSC_FREQ_MIN;
    if(freq > OSC_FREQ_MAX) freq = OSC_FREQ_MAX;
    filter_1p_lo_in(freq3Lp, freq);
    freq3 = freq;
//    freq = &trash;
}


static inline fract32 lookup_wave(const fix16 idx, const fract32 wave) {
#if 1
  u32 idxA = wave >> WAVE_TAB_RSHIFT;
  u32 idxB = idxA + 1;

  fract32 mul = (wave & WAVE_TAB_MASK) << WAVE_TAB_LSHIFT;
  fract32 mulInv = sub_fr1x32(FR32_MAX, mul);
#else
#endif

  return add_fr1x32( 
		    mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxA], WAVE_TAB_SIZE, idx), mulInv),
		    mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxB], WAVE_TAB_SIZE, idx), mul)
		     );
}
    

static void calc_frame(void) {
    amp0 = filter_1p_lo_next(amp0Lp);
    amp1 = filter_1p_lo_next(amp1Lp);
    amp2 = filter_1p_lo_next(amp2Lp);
    amp3 = filter_1p_lo_next(amp3Lp);

    wave0 = filter_1p_lo_next(wave0Lp);
    wave1 = filter_1p_lo_next(wave1Lp);
    wave2 = filter_1p_lo_next(wave2Lp);
    wave3 = filter_1p_lo_next(wave3Lp);

    osc0 = lookup_wave(idx0, wave0);
    osc1 = lookup_wave(idx1, wave1);
    osc2 = lookup_wave(idx2, wave2);
    osc3 = lookup_wave(idx3, wave3);
        
    tmp0 = add_fr1x32(mult_fr1x32x32(osc0, amp0), 1);  
    tmp1 = add_fr1x32(mult_fr1x32x32(osc1, amp1), 1);
    tmp2 = add_fr1x32(mult_fr1x32x32(osc2, amp2), 1);
    tmp3 = add_fr1x32(mult_fr1x32x32(osc3, amp3), 1);

//summed output version
/*
    fract32 tmp;
    frameVal = add_fr1x32(
                          mult_fr1x32x32(osc0, amp0),
                          mult_fr1x32x32(osc1, amp1)
                          );
    tmp = frameVal;
    frameVal = add_fr1x32(
                          mult_fr1x32x32(osc2, amp2),
                          mult_fr1x32x32(osc3, amp3)
                          );
    frameVal = frameVal + tmp;
*/

    if(!(freq0Lp->sync)) {
        inc0 = fix16_mul(filter_1p_lo_next(freq0Lp), ips);
    } 
    if(!(freq1Lp->sync)) {
        inc1 = fix16_mul(filter_1p_lo_next(freq1Lp), ips);
    } 
    if(!(freq2Lp->sync)) {
        inc2 = fix16_mul(filter_1p_lo_next(freq2Lp), ips);
    } 
    if(!(freq3Lp->sync)) {
        inc3 = fix16_mul(filter_1p_lo_next(freq3Lp), ips);
    }     
 
    idx0 = fix16_add(idx0, inc0);
    while(idx0 > WAVE_TAB_MAX16) {
        idx0 = fix16_sub(idx0, WAVE_TAB_MAX16);
    }
    
    idx1 = fix16_add(idx1, inc1);
    while(idx1 > WAVE_TAB_MAX16) { 
        idx1 = fix16_sub(idx1, WAVE_TAB_MAX16); 
    }

    idx2 = fix16_add(idx2, inc2);
    while(idx2 > WAVE_TAB_MAX16) { 
        idx2 = fix16_sub(idx2, WAVE_TAB_MAX16);
    }

    idx3 = fix16_add(idx3, inc3);
    while(idx3 > WAVE_TAB_MAX16) { 
        idx3 = fix16_sub(idx3, WAVE_TAB_MAX16);
    }
}


//external functions
void module_init(void) {
    data = (prgmData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "aleph-prgm");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;

    sr = SAMPLERATE;
    ips = fix16_from_float((f32)WAVE_TAB_SIZE / (f32)sr);

    amp0 = amp1 = amp2 = amp3 = INT32_MAX >> 4;

//    freq0 = freq1 = freq2 = freq3 = FIX16_ONE; 

    freq0 = fix16_from_int(95);
    freq1 = fix16_from_int(190);
    freq2 = fix16_from_int(380);
    freq3 = fix16_from_int(760);
    
    idx0 = idx1 = idx2 = idx3 = 0;

    ioAmp0 = FR32_MAX;
    ioAmp1 = FR32_MAX;
    ioAmp2 = FR32_MAX;
    ioAmp3 = FR32_MAX;

    gModuleData->paramData[eParamFreq0].value = freq0;
    gModuleData->paramData[eParamFreq1].value = freq1;
    gModuleData->paramData[eParamFreq2].value = freq2;
    gModuleData->paramData[eParamFreq3].value = freq3;

    gModuleData->paramData[eParamAmp0].value = amp0;
    gModuleData->paramData[eParamAmp1].value = amp1;
    gModuleData->paramData[eParamAmp2].value = amp2;
    gModuleData->paramData[eParamAmp3].value = amp3;

    gModuleData->paramData[eParamIoAmp0].value = ioAmp0;
    gModuleData->paramData[eParamIoAmp1].value = ioAmp1;
    gModuleData->paramData[eParamIoAmp2].value = ioAmp2;
    gModuleData->paramData[eParamIoAmp3].value = ioAmp3;

    freq0Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(freq0Lp, freq0);
    freq1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(freq1Lp, freq1);
    freq2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(freq2Lp, freq2);
    freq3Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(freq3Lp, freq3);

    wave0Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(wave0Lp, wave0);
    wave1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(wave1Lp, wave1);
    wave2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(wave2Lp, wave2);
    wave3Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(wave3Lp, wave3);

    amp0Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(amp0Lp, amp0);
    amp1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(amp1Lp, amp1);
    amp2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(amp2Lp, amp2);
    amp3Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
    filter_1p_lo_init(amp2Lp, amp3);
 
    inc0 = fix16_mul(freq0, ips);
    inc1 = fix16_mul(freq1, ips);
    inc2 = fix16_mul(freq2, ips);
    inc3 = fix16_mul(freq3, ips);
}


void module_deinit(void) {
    free(freq0Lp);
    free(freq1Lp);
    free(freq2Lp);
    free(freq3Lp);
    free(wave0Lp);
    free(wave1Lp);
    free(wave2Lp);
    free(wave3Lp);
    free(amp0Lp);
    free(amp1Lp);
    free(amp2Lp);
    free(amp3Lp);
}


extern u32 module_get_num_params(void) {
  return eParamNumParams;
}


void module_process_frame(void) {
  calc_frame();

//summed output version
/*
  out[0] = add_fr1x32(frameVal, mult_fr1x32x32(in[0], ioAmp0));
  out[1] = add_fr1x32(frameVal, mult_fr1x32x32(in[1], ioAmp1));
  out[2] = add_fr1x32(frameVal, mult_fr1x32x32(in[2], ioAmp2));
  out[3] = add_fr1x32(frameVal, mult_fr1x32x32(in[3], ioAmp3));
*/
    out[0] = add_fr1x32(tmp0, mult_fr1x32x32(in[0], ioAmp0));
    out[1] = add_fr1x32(tmp1, mult_fr1x32x32(in[1], ioAmp1));
    out[2] = add_fr1x32(tmp2, mult_fr1x32x32(in[2], ioAmp2));
    out[3] = add_fr1x32(tmp3, mult_fr1x32x32(in[3], ioAmp3));
}


void module_set_param(u32 idx, ParamValue v) {
    switch(idx) {
        case eParamFreq0:
            set_freq0(v);
            break;
        case eParamFreq1:
            set_freq1(v);
            break;
        case eParamFreq2:
            set_freq2(v);
            break;
        case eParamFreq3:
            set_freq3(v);
            break;

        case eParamWave0:
            filter_1p_lo_in(wave0Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
            break;
        case eParamWave1:
            filter_1p_lo_in(wave1Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
            break;
        case eParamWave2:
            filter_1p_lo_in(wave2Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
            break;
        case eParamWave3:
            filter_1p_lo_in(wave3Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
            break;

        case eParamAmp0:
            filter_1p_lo_in(amp0Lp, v);
            break;
        case eParamAmp1:
            filter_1p_lo_in(amp1Lp, v);
            break;
        case eParamAmp2:
            filter_1p_lo_in(amp2Lp, v);
            break;
        case eParamAmp3:
            filter_1p_lo_in(amp3Lp, v);
            break;

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
