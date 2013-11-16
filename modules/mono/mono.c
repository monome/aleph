/* mono.c
 * aleph-bfin
 * 
 * PM chain synthesizer, 2 oscs, on all outputs,
 * plus i/o loop on each i/o pair
 */

// std
#include <math.h>
#include <stdlib.h>
#include <string.h>
// aleph-common
#include "fix.h"
//#include "simple_string.h"
// audio lib
#include "env.h"
//#include "filters.h"
#include "filter_1p.h"
#include "table.h"
#include "conversion.h"
// bfin
#ifdef ARCH_BFIN
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>
#else
// linux
#include "fract32_emu.h"
#include "audio.h"

/////// dbg
#include <stdio.h>

#endif

#include "module.h"
#include "module_custom.h"
#include "types.h"

//---------- defines
// ranges - all are fix16
#define OSC_FREQ_MIN 0x200000      // 32
#define OSC_FREQ_MAX 0x40000000    // 16384
#define RATIO_MIN 0x2000     // 1/8
#define RATIO_MAX 0x80000    // 8
#define ENV_DUR_MIN 0x0040   // 1/1024
#define ENV_DUR_MAX 0x100000 // 32
#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 128

//-------- data types


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _monoFmData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} monoFmData;

//--- wavetable
#define WAVE_TAB_SIZE 	2048
#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)
#define WAVE_TAB_MAX16 	(WAVE_TAB_SIZE * FIX16_ONE - 1)

//-------------------------
//----- extern vars (initialized here)
moduleData * gModuleData; // module data

//-----------------------
//------ static variables

// pointer to local module data, initialize/v at top of SDRAM
static monoFmData * monoData;

//-- static allocation (SRAM) for variables that are small and/or frequently accessed:

static   fract32   tab1[WAVE_TAB_SIZE]; // wavetable1
static   fract32   tab2[WAVE_TAB_SIZE]; // wavetable1

static u32       sr;            // sample rate
static fix16     ips;        // index change per sample

static fix16     idx1;          // current phase (fractional idx) (primary)
static fix16     idx2;          // current phase (fractional idx) (secondary)
static fix16     idx1Mod;       // modulated primary index
static fix16     inc1;          // idx change at current frequency (primary)
static fix16     inc2;          // idx change at current frequency (secondary)
static fract32   osc1;          // primary oscillator output
static fract32   osc2;          // secondary oscillator output
static fract32   frameVal;      // output value
static fract32   pm;           // phase modulation depth

static fix16     freq1;            // base frequency (primary oscillator)
static fix16     freq2;            // base frequency (secondary oscillator)
static fix16     ratio2;         // frequency ratio for secondary oscillator
static u8        track;         // pitch-tracking flag

static fract32   wave1;          // waveshape (primary)
static fract32   wave2;          // waveshape (secondary)

static fract32   amp1;          // amplitude (primary)
static fract32   amp2;          // amplitude (secondary)

static env_asr*  env;           // ASR amplitude envelope
 
static filter_1p_lo* freq1Lp;  // 1plp smoother for freq1
static filter_1p_lo* freq2Lp;  // 1plp smoother for freq2
static filter_1p_lo* pmLp;    // 1plp smoother for pm
static filter_1p_lo* wave1Lp; // 1plp smoother for wave1
static filter_1p_lo* wave2Lp; // 1plp smoother for wave2
static filter_1p_lo* amp1Lp;  // 1plp smoother for amp1
static filter_1p_lo* amp2Lp;  // 1plp smoother for amp2

// dry I->O amplitudes
static fract32 ioAmp0;
static fract32 ioAmp1;
static fract32 ioAmp2;
static fract32 ioAmp3;

//----------------------
//----- static function declaration
// set primary freq
static inline void set_freq1(fix16 freq);
// set secondary pitch ratio - enters pitch tracking mode
static inline void set_ratio2(const fix16 ratio);
// set secondary frequency - leaves pitch tracking mode
static inline void set_freq2(fix16 freq);
// double-lookup and interpolate
static inline fract32 lookup_wave(const fix16 idx, const fract32 wave);
// frame calculation
static void calc_frame(void);
// set all parameter descriptors
static void fill_param_desc(void);

//----------------------
//----- static functions

// set primary freq
static inline void set_freq1(fix16 freq) {  



  if( freq < OSC_FREQ_MIN ) freq = OSC_FREQ_MIN;
  if( freq > OSC_FREQ_MAX ) freq = OSC_FREQ_MAX;
  filter_1p_lo_in(freq1Lp, freq);
  freq1 = freq;
  if(track) {
    filter_1p_lo_in(freq2Lp, fix16_mul(freq1, ratio2) ) ;
  }
}

// set secondary pitch ratio - enters pitch tracking mode
static inline void set_ratio2(const fix16 ratio) {
  ratio2 = ratio;
  if (ratio2 > RATIO_MAX) ratio2 = RATIO_MAX;
  if (ratio2 < RATIO_MIN) ratio2 = RATIO_MIN;
  filter_1p_lo_in(freq2Lp, fix16_mul(freq1, ratio2));
  track = 1;
}

// set secondary frequency - leaves pitch tracking mode
static inline void set_freq2(fix16 freq) {
  if( freq < OSC_FREQ_MIN ) freq = OSC_FREQ_MIN;
  if( freq > OSC_FREQ_MAX ) freq = OSC_FREQ_MAX;
  filter_1p_lo_in(freq2Lp, freq);
  track = 0;
}

// double-lookup and interpolate
static inline fract32 lookup_wave(const fix16 idx, const fract32 wave) {
  ///// FIXME: this is far from optimized.
  //// add refactored double-lookup method to table.h
  const fract32 waveInv = sub_fr1x32(INT32_MAX, wave);
  return add_fr1x32( 
		    mult_fr1x32x32(table_lookup_idx(tab1, WAVE_TAB_SIZE, idx), waveInv),
		    mult_fr1x32x32(table_lookup_idx(tab2, WAVE_TAB_SIZE, idx), wave)
		     );
}

// frame calculation
static void calc_frame(void) {
  // ----- smoothers:
  // pm
  pm = filter_1p_lo_next(pmLp);

  // amp1
  amp1 = filter_1p_lo_next(amp1Lp);
  // amp2
  amp2 = filter_1p_lo_next(amp2Lp);

  // wave1
  wave1 = filter_1p_lo_next(wave1Lp);
  // wave2
  wave2 = filter_1p_lo_next(wave2Lp);
  // lookup osc2
  osc2 = lookup_wave(idx2, wave2);
  
  /// use osc2 output as phase modulation, scaled to tablesize
  idx1Mod = fix16_add(
		      idx1,
		      fix16_mul(
				FRACT_FIX16(mult_fr1x32x32(osc2, pm)),
				WAVE_TAB_MAX16)
		      );

  // wrap negative
  while (BIT_SIGN(idx1Mod)) {
    idx1Mod = fix16_add(idx1Mod, WAVE_TAB_MAX16);
  }

  // wrap positive
  while(idx1Mod > WAVE_TAB_MAX16) { 
    idx1Mod = fix16_sub(idx1Mod, WAVE_TAB_MAX16); 
  }

  // lookup osc1
  osc1 = lookup_wave(idx1Mod, wave1);

  // apply amplitudes and sum 
  frameVal = add_fr1x32(
			mult_fr1x32x32(osc1, amp1),
			mult_fr1x32x32(osc2, amp2)
			);


  // increment and apply envelope
    frameVal = mult_fr1x32x32(frameVal, env_asr_next(env));

  // increment and apply freq smoothers
  if(!(freq1Lp->sync)) {
    inc1 = fix16_mul(filter_1p_lo_next(freq1Lp), ips);
    //    set_freq
  } 
  if(!(freq2Lp->sync)) {
    inc2 = fix16_mul(filter_1p_lo_next(freq2Lp), ips);
  } 

  // increment phasor idx and wrap
  idx1 = fix16_add(idx1, inc1);
  while(idx1 > WAVE_TAB_MAX16) { 
    idx1 = fix16_sub(idx1, WAVE_TAB_MAX16);
  }

  idx2 = fix16_add(idx2, inc2);
  while(idx2 > WAVE_TAB_MAX16) { 
    idx2 = fix16_sub(idx2, WAVE_TAB_MAX16); 
  }
}

//----------------------
//----- external functions

void module_init(void) {

  // init module/param descriptor
  // intialize local data at start of SDRAM
  monoData = (monoFmData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
  gModuleData = &(monoData->super);
  gModuleData->paramDesc = monoData->mParamDesc;
  gModuleData->paramData = monoData->mParamData;
  gModuleData->numParams = eParamNumParams;

  fill_param_desc();

  // init params
  sr = SAMPLERATE;
  ips = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );

  track = 1;
  amp1 = amp2 = INT32_MAX >> 1;
  freq1 = fix16_from_int(220);
  freq2 = fix16_from_int(330);
  ratio2 = fix16_from_float(1.5);
  idx1 = idx2 = 0;

  ioAmp0 = FR32_MAX;
  ioAmp1 = FR32_MAX;
  ioAmp2 = FR32_MAX;
  ioAmp3 = FR32_MAX;

  // init wavetables
  table_fill_harm(tab1, WAVE_TAB_SIZE, 1, 1.f, 0);
  table_fill_harm(tab2, WAVE_TAB_SIZE, 5, 0.5, 1);

  // allocate envelope
  env = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(env);

  env_asr_set_atk_shape(env, float_to_fr32(0.5));
  env_asr_set_rel_shape(env, float_to_fr32(0.5));
  env_asr_set_atk_dur(env, 1000);
  env_asr_set_rel_dur(env, 10000);

  // allocate smoothers
  freq1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( freq1Lp, freq1 );

  freq2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( freq2Lp, freq2 );

  pmLp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( pmLp, pm );

  wave1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( wave1Lp, wave1 );

  wave2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( wave2Lp, wave2 );

  amp1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp1Lp, amp1 );

  amp2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp2Lp, amp2 );

  // initialize osc phasor increment
  inc1 = fix16_mul(freq1, ips);
  inc2 = fix16_mul(freq2, ips);
;
}

// de-init
void module_deinit(void) {
  free(env);
  free(freq1Lp);
  free(freq2Lp);
  free(pmLp);
  free(wave1Lp);
  free(wave2Lp);
  free(amp1Lp);
  free(amp2Lp);

}

// set parameter by value (fix16)
void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
  case eParamFreq1:
    set_freq1(v);
    break;
  case eParamRatio2:
    set_ratio2(v);
    break;
  case eParamFreq2:
    set_freq2(v);
    break;
  case eParamWave1:
    filter_1p_lo_in(wave1Lp, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamWave2:
    filter_1p_lo_in(wave2Lp, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamPm:
    filter_1p_lo_in(pmLp, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamAmp1:
    filter_1p_lo_in(amp1Lp, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamAmp2:
    filter_1p_lo_in(amp2Lp, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamGate:
     env_asr_set_gate(env, v > 0);
    break;
  case eParamAtkDur:
    env_asr_set_atk_dur(env, sec_to_frames_trunc(v));
    break;
  case eParamRelDur:
    env_asr_set_rel_dur(env, sec_to_frames_trunc(v));
    break;
  case eParamAtkCurve:
    env_asr_set_atk_shape(env, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamRelCurve:
    env_asr_set_atk_shape(env, FIX16_FRACT_TRUNC(BIT_ABS(v)));
    break;
  case eParamFreq1Smooth:
    filter_1p_lo_set_hz(freq1Lp, v);
    break;
  case eParamFreq2Smooth:
    filter_1p_lo_set_hz(freq2Lp, v);
    break;
  case eParamPmSmooth:
    filter_1p_lo_set_hz(pmLp, v);
    break;
  case eParamWave1Smooth:
    filter_1p_lo_set_hz(wave1Lp, v);
    break;
  case eParamWave2Smooth:
    filter_1p_lo_set_hz(wave2Lp, v);
    break;
  case eParamAmp1Smooth:
    filter_1p_lo_set_hz(amp1Lp, v);
    break;
  case eParamAmp2Smooth:
    filter_1p_lo_set_hz(amp2Lp, v);
    break;
  case eParamIoAmp0:
    ioAmp0 = FIX16_FRACT_TRUNC(v);
    break;
  case eParamIoAmp1:
    ioAmp1 = FIX16_FRACT_TRUNC(v);
    break;
  case eParamIoAmp2:
    ioAmp2 = FIX16_FRACT_TRUNC(v);
    break;
  case eParamIoAmp3:
    ioAmp3 = FIX16_FRACT_TRUNC(v);
    break;
  default:
    break;
  }
}

// get number of parameters
extern u32 module_get_num_params(void) {
  return eParamNumParams;
}

// frame callback
void module_process_frame(void) {
  calc_frame();
  out[0] = add_fr1x32(frameVal, mult_fr1x32x32(in[0], ioAmp0));
  out[1] = add_fr1x32(frameVal, mult_fr1x32x32(in[1], ioAmp1));
  out[2] = add_fr1x32(frameVal, mult_fr1x32x32(in[2], ioAmp2));
  out[3] = add_fr1x32(frameVal, mult_fr1x32x32(in[3], ioAmp3));
}

static u8 ledstate = 0;
u8 module_update_leds(void) {
  return ledstate;
}

static void fill_param_desc(void) {
  strcpy(gModuleData->paramDesc[eParamFreq1].label, "osc 1 freq");
  strcpy(gModuleData->paramDesc[eParamFreq1].unit, "freq");
  gModuleData->paramDesc[eParamFreq1].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamFreq1].min = OSC_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq1].max = OSC_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamFreq2].label, "osc 2 freq");
  strcpy(gModuleData->paramDesc[eParamFreq2].unit, "freq");
  gModuleData->paramDesc[eParamFreq2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamFreq2].min = OSC_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq2].max = OSC_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamRatio2].label, "osc 2 ratio");
  strcpy(gModuleData->paramDesc[eParamRatio2].unit, "");
  gModuleData->paramDesc[eParamRatio2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRatio2].min = RATIO_MIN;
  gModuleData->paramDesc[eParamRatio2].max = RATIO_MAX;
  
  strcpy(gModuleData->paramDesc[eParamAmp1].label, "amplitude 1");
  strcpy(gModuleData->paramDesc[eParamAmp1].unit, "");
  gModuleData->paramDesc[eParamAmp1].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAmp1].min = 0;
  gModuleData->paramDesc[eParamAmp1].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamAmp2].label, "amplitude 2");
  strcpy(gModuleData->paramDesc[eParamAmp2].unit, "");
  gModuleData->paramDesc[eParamAmp2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAmp2].min = 0;
  gModuleData->paramDesc[eParamAmp2].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamIoAmp0].label, "I/O amp 1");
  strcpy(gModuleData->paramDesc[eParamIoAmp0].unit, "");
  gModuleData->paramDesc[eParamIoAmp0].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamIoAmp0].min = 0;
  gModuleData->paramDesc[eParamIoAmp0].max = FIX16_ONE - 1;

  strcpy(gModuleData->paramDesc[eParamIoAmp1].label, "I/O amp 2");
  strcpy(gModuleData->paramDesc[eParamIoAmp1].unit, "");
  gModuleData->paramDesc[eParamIoAmp1].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamIoAmp1].min = 0;
  gModuleData->paramDesc[eParamIoAmp1].max = FIX16_ONE - 1;

  strcpy(gModuleData->paramDesc[eParamIoAmp2].label, "I/O amp 3");
  strcpy(gModuleData->paramDesc[eParamIoAmp2].unit, "");
  gModuleData->paramDesc[eParamIoAmp2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamIoAmp2].min = 0;
  gModuleData->paramDesc[eParamIoAmp2].max = FIX16_ONE - 1;

  strcpy(gModuleData->paramDesc[eParamIoAmp3].label, "I/O amp 4");
  strcpy(gModuleData->paramDesc[eParamIoAmp3].unit, "");
  gModuleData->paramDesc[eParamIoAmp3].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamIoAmp3].min = 0;
  gModuleData->paramDesc[eParamIoAmp3].max = FIX16_ONE - 1;

  strcpy(gModuleData->paramDesc[eParamPm].label, "phase mod depth");
  strcpy(gModuleData->paramDesc[eParamPm].unit, "");
  gModuleData->paramDesc[eParamPm].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamPm].min = 0;
  gModuleData->paramDesc[eParamPm].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamWave1].label, "waveshape 1");
  strcpy(gModuleData->paramDesc[eParamWave1].unit, "");
  gModuleData->paramDesc[eParamWave1].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamWave1].min = 0;
  gModuleData->paramDesc[eParamWave1].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamWave2].label, "waveshape 2");
  strcpy(gModuleData->paramDesc[eParamWave2].unit, "");
  gModuleData->paramDesc[eParamWave2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamWave2].min = 0;
  gModuleData->paramDesc[eParamWave2].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamGate].label, "gate");
  strcpy(gModuleData->paramDesc[eParamGate].unit, "");
  gModuleData->paramDesc[eParamGate].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamGate].min = 0;
  gModuleData->paramDesc[eParamGate].max = FIX16_ONE;
  
  strcpy(gModuleData->paramDesc[eParamAtkDur].label, "amp env attack");
  strcpy(gModuleData->paramDesc[eParamAtkDur].unit, "freq");
  gModuleData->paramDesc[eParamAtkDur].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAtkDur].min = 0;
  gModuleData->paramDesc[eParamAtkDur].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamRelDur].label, "amp env release");
  strcpy(gModuleData->paramDesc[eParamRelDur].unit, "freq");
  gModuleData->paramDesc[eParamRelDur].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRelDur].min = 0;
  gModuleData->paramDesc[eParamRelDur].max = FIX16_ONE;
  
  strcpy(gModuleData->paramDesc[eParamAtkCurve].label, "amp env atk curve");
  strcpy(gModuleData->paramDesc[eParamAtkCurve].unit, "freq");
  gModuleData->paramDesc[eParamAtkCurve].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAtkCurve].min = 0;
  gModuleData->paramDesc[eParamAtkCurve].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamRelCurve].label, "amp env rel curve");
  strcpy(gModuleData->paramDesc[eParamRelCurve].unit, "freq");
  gModuleData->paramDesc[eParamRelCurve].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRelCurve].min = 0;
  gModuleData->paramDesc[eParamRelCurve].max = FIX16_ONE - 1;
  
  strcpy(gModuleData->paramDesc[eParamFreq1Smooth].label, "freq 1 smoothing");
  strcpy(gModuleData->paramDesc[eParamFreq1Smooth].unit, "freq");
  gModuleData->paramDesc[eParamFreq1Smooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamFreq1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq1Smooth].max = SMOOTH_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamFreq2Smooth].label, "freq 2 smoothing");
  strcpy(gModuleData->paramDesc[eParamFreq2Smooth].unit, "");
  gModuleData->paramDesc[eParamFreq2Smooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamFreq2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamFreq2Smooth].max = SMOOTH_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamPmSmooth].label, "phase mod smoothing");
  strcpy(gModuleData->paramDesc[eParamPmSmooth].unit, "");
  gModuleData->paramDesc[eParamPmSmooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamPmSmooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamPmSmooth].max = SMOOTH_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamWave1Smooth].label, "wave 1 smoothing");
  strcpy(gModuleData->paramDesc[eParamWave1Smooth].unit, "");
  gModuleData->paramDesc[eParamWave1Smooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamWave1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWave1Smooth].max = SMOOTH_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamWave2Smooth].label, "wave 2 smoothing");
  strcpy(gModuleData->paramDesc[eParamWave2Smooth].unit, "");
  gModuleData->paramDesc[eParamWave2Smooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamWave2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamWave2Smooth].max = SMOOTH_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamAmp1Smooth].label, "amp 1 smoothing");
  strcpy(gModuleData->paramDesc[eParamAmp1Smooth].unit, "freq");
  gModuleData->paramDesc[eParamAmp1Smooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAmp1Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamAmp1Smooth].max = SMOOTH_FREQ_MAX;
  
  strcpy(gModuleData->paramDesc[eParamAmp2Smooth].label, "amp 2 smoothing");
  strcpy(gModuleData->paramDesc[eParamAmp2Smooth].unit, "freq");
  gModuleData->paramDesc[eParamAmp2Smooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAmp2Smooth].min = SMOOTH_FREQ_MIN;
  gModuleData->paramDesc[eParamAmp2Smooth].max = SMOOTH_FREQ_MAX;
}
