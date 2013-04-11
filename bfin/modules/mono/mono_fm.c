/* mono.c
 * nullp
 * 
 * a monosynth module using FM chain synthesis (dx-7 style)
 */

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "simple_string.h"

#ifdef ARCH_BFIN
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>
#else
#include "fract32_emu.h"
#include "audio.h"
#endif

// audio lib
#include "env.h"
#include "filters.h"
#include "module.h"
#include "table.h"
#include "types.h"

/// DEBUG
//static u32 framecount = 0;

//typedef union { u32 u; s32 s; fix16 fix; fract32 fr; } pval;

///// inputs
enum params {
  eParamHz1,
  eParamHz2,
  eParamRatio2,
  eParamPm,
  eParamWave1,
  eParamWave2,
  eParamAmp1,
  eParamAmp2,
  eParamGate,
  eParamAtkDur,
  eParamRelDur,
  eParamAtkCurve,
  eParamRelCurve,
  eParamHz1Smooth,
  eParamHz2Smooth,
  eParamPmSmooth,
  eParamWave1Smooth,
  eParamWave2Smooth,
  eParamAmp1Smooth,
  eParamAmp2Smooth,
  
  eParamNumParams
};

// hz is fix16
#define HZ_MIN 0x200000   // 32
#define HZ_MAX 0x20000000 // 8192
#define RATIO_MIN 0x2000 // 1/8
#define RATIO_MAX 0x80000 // 8

//--- wavetable
#define WAVE_TAB_SIZE 512
#define WAVE_TAB_SIZE_1 511
//#define WAVE_TAB_MAX16 0x1ff0000 // 511 * 0x10000
#define WAVE_TAB_MAX16 0x1ffffff // (512 * 0x10000) - 1

//-------------------------
//----- extern vars (initialized here)
moduleData_t * moduleData; // module data

/// datastructure for stuff that will live in SDRAM
/* typedef struct _mono_data { */
/*   fract32   tab1[WAVE_TAB_SIZE]; // wavetable1 */
/*   fract32   tab2[WAVE_TAB_SIZE]; // wavetable2 */
/* } mono_data; */

//-----------------------
//------ static variables
/* static mono_data * monoData = (mono_data*)SDRAM_ADDRESS; */
/* static fract32* tab1; */
/* static fract32* tab2; */

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

static fix16     hz1;            // base frequency (primary oscillator)
static fix16     hz2;            // base frequency (secondary oscillator)
static fix16     ratio2;         // frequency ratio for secondary oscillator
static u8        track;         // pitch-tracking flag

static fract32   wave1;          // waveshape (primary)
static fract32   wave2;          // waveshape (secondary)

static fract32   amp1;          // amplitude (primary)
static fract32   amp2;          // amplitude (secondary)

static env_asr*  env;           // ASR amplitude envelope
 
static filter_1p_fix16* hz1Lp;  // 1plp smoother for hz1
static filter_1p_fix16* hz2Lp;  // 1plp smoother for hz2
static filter_1p_fr32* pmLp;    // 1plp smoother for pm
static filter_1p_fr32* wave1Lp; // 1plp smoother for wave1
static filter_1p_fr32* wave2Lp; // 1plp smoother for wave2
static filter_1p_fr32* amp1Lp;  // 1plp smoother for amp1
static filter_1p_fr32* amp2Lp;  // 1plp smoother for amp2


//-----------------------
//----- fwd declaration 
// from table.h
extern inline fract32 table_lookup_idx(fract32* tab, u32 size, fix16 idx);

//----------------------
//----- static functions

// set hz
static inline void set_hz1(fix16 hz) {  
  if( hz < HZ_MIN ) hz = HZ_MIN;
  if( hz > HZ_MAX ) hz = HZ_MAX;
  filter_1p_fix16_in(hz1Lp, hz);
  hz1 = hz;
  if(track) {
    filter_1p_fix16_in(hz2Lp, fix16_mul(hz1, ratio2) ) ;
  }
}

// set secondary pitch ratio - enters pitch tracking mode
static inline void set_ratio2(const fix16 ratio) {
  ratio2 = ratio;
  if (ratio2 > RATIO_MAX) ratio2 = RATIO_MAX;
  if (ratio2 < RATIO_MIN) ratio2 = RATIO_MIN;
  filter_1p_fix16_in(hz2Lp, fix16_mul(hz1, ratio2));
  track = 1;
}

// set secondary frequency - leaves pitch tracking mode
static inline void set_hz2(fix16 hz) {
  if( hz < HZ_MIN ) hz = HZ_MIN;
  if( hz > HZ_MAX ) hz = HZ_MAX;
  filter_1p_fix16_in(hz2Lp, hz);
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
  pm = filter_1p_fr32_next(pmLp);
  // amp1
  amp1 = filter_1p_fr32_next(amp1Lp);
  // amp2
  amp2 = filter_1p_fr32_next(amp2Lp);
  // wave1
  wave1 = filter_1p_fr32_next(wave1Lp);
  // wave2
  wave2 = filter_1p_fr32_next(wave2Lp);

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
  while (BSIGN(idx1Mod)) {
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
  //  frameVal = mult_fr1x32x32(frameVal, env_asr_next(env));

  // increment and apply hz smoothers
  if(!(hz1Lp->sync)) {
    inc1 = fix16_mul(filter_1p_fix16_next(hz1Lp), ips);
  } 
  if(!(hz2Lp->sync)) {
    inc2 = fix16_mul(filter_1p_fix16_next(hz2Lp), ips);
  } 

  // increment idx and wrap
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
#ifdef ARCH_BFIN 
  moduleData = (moduleData_t*)SDRAM_ADDRESS;
#else
  moduleData = (moduleData_t*)malloc(sizeof(moduleData_t));
#endif

  moduleData->numParams = eParamNumParams;
  moduleData->paramDesc = (ParamDesc*)malloc(eParamNumParams * sizeof(ParamDesc));
  moduleData->paramData = (ParamData*)malloc(eParamNumParams * sizeof(ParamData));

  strcpy(moduleData->paramDesc[eParamHz1].label, "osc 1 hz");
  strcpy(moduleData->paramDesc[eParamHz2].label, "osc 2 hz");
  strcpy(moduleData->paramDesc[eParamRatio2].label, "osc 2 ratio");
  strcpy(moduleData->paramDesc[eParamPm].label, "phase mod depth");
  strcpy(moduleData->paramDesc[eParamWave1].label, "waveshape 1");
  strcpy(moduleData->paramDesc[eParamWave2].label, "waveshape 2");
  strcpy(moduleData->paramDesc[eParamAmp1].label, "amplitude 1");
  strcpy(moduleData->paramDesc[eParamAmp2].label, "amplitude 2");
  strcpy(moduleData->paramDesc[eParamGate].label, "gate");
  strcpy(moduleData->paramDesc[eParamAtkDur].label, "amp env attack");
  strcpy(moduleData->paramDesc[eParamRelDur].label, "amp env release");
  strcpy(moduleData->paramDesc[eParamAtkCurve].label, "amp env atk curve");
  strcpy(moduleData->paramDesc[eParamRelCurve].label, "amp env rel curve");
  strcpy(moduleData->paramDesc[eParamHz1Smooth].label, "hz 1 smoothing");
  strcpy(moduleData->paramDesc[eParamHz2Smooth].label, "hz 2 smoothing");
  strcpy(moduleData->paramDesc[eParamPmSmooth].label, "phase mod smoothing");
  strcpy(moduleData->paramDesc[eParamWave1Smooth].label, "wave 1 smoothing");
  strcpy(moduleData->paramDesc[eParamWave2Smooth].label, "wave 2 smoothing");
  strcpy(moduleData->paramDesc[eParamAmp1Smooth].label, "amp 1 smoothing");
  strcpy(moduleData->paramDesc[eParamAmp2Smooth].label, "amp 2 smoothing");

  // init params
  sr = SAMPLERATE;
  track = 1;
  ips = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );
  amp1 = amp2 = INT32_MAX >> 1;
  hz1 = fix16_from_int(220);
  hz2 = fix16_from_int(330);
  ratio2 = fix16_from_float(1.5);
  idx1 = idx2 = 0;

  // init wavetables
  /* tab1 = monoData->tab1; */
  /* tab2 = monoData->tab2; */
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
  hz1Lp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( hz1Lp, SAMPLERATE, 16 << 16, hz1 );

  hz2Lp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( hz2Lp, SAMPLERATE, 32 << 16, hz2 );

  pmLp = (filter_1p_fr32*)malloc(sizeof(filter_1p_fr32));
  filter_1p_fr32_init( pmLp, SAMPLERATE, 0x4000, pm );

  wave1Lp = (filter_1p_fr32*)malloc(sizeof(filter_1p_fr32));
  filter_1p_fr32_init( wave1Lp, SAMPLERATE, 0xa0000, wave1 );

  wave2Lp = (filter_1p_fr32*)malloc(sizeof(filter_1p_fr32));
  filter_1p_fr32_init( wave2Lp,  SAMPLERATE, 0xa0000, wave2 );

  amp1Lp = (filter_1p_fr32*)malloc(sizeof(filter_1p_fr32));
  filter_1p_fr32_init( amp1Lp, SAMPLERATE, 0xa0000, amp1 );

  amp2Lp = (filter_1p_fr32*)malloc(sizeof(filter_1p_fr32));
  filter_1p_fr32_init( amp2Lp, SAMPLERATE, 0xa0000, amp2 );

  inc1 = fix16_mul(hz1, ips);
  inc2 = fix16_mul(hz2, ips);

  /// DEBUG
  //printf("\n\n module init debug \n\n");

  ////// TEST
  env_asr_set_gate(env, 1);
}

// de-init
void module_deinit(void) {
  free(env);
  free(hz1Lp);
  free(hz2Lp);
  free(pmLp);
  free(wave1Lp);
  free(wave2Lp);
  free(amp1Lp);
  free(amp2Lp);
}

// set parameter by value
///// FIXME: the represeentation of this value is totally arbitrary!
void module_set_param(u32 idx, pval v) {
  switch(idx) {
  case eParamHz1:
    // set_hz1(fix16_from_float(v));
    set_hz1(v.fix);
    break;
  case eParamRatio2:
    //    set_ratio2(fix16_from_float(v));
    set_ratio2(v.fix);
    break;
  case eParamHz2:
    //    set_hz2(fix16_from_float(v));
    set_hz2(v.fix);
    break;
  case eParamWave1:
    //    filter_1p_fr32_in(wave1Lp, float_to_fr32(v));
    filter_1p_fr32_in(wave1Lp, v.fr);
    break;
  case eParamWave2:
    //    filter_1p_fr32_in(wave2Lp, float_to_fr32(v));
    filter_1p_fr32_in(wave2Lp, v.fr);
    break;
  case eParamPm:
    // scale to [0, 0.5]
    //    filter_1p_fr32_in(pmLp, float_to_fr32(v) >> 1);
    //    filter_1p_fr32_in(pmLp, v.fr >> 1);
    filter_1p_fr32_in(pmLp, v.fix);
    break;
  case eParamAmp1:
    //    filter_1p_fr32_in(amp1Lp, float_to_fr32(v));
    filter_1p_fr32_in(amp1Lp, v.fr);
    break;
  case eParamAmp2:
    //    filter_1p_fr32_in(amp2Lp, float_to_fr32(v));
    filter_1p_fr32_in(amp2Lp, v.fr);
    break;
  case eParamGate:
     env_asr_set_gate(env, v.s > 0);
    break;
  case eParamAtkDur:
    env_asr_set_atk_dur(env, v.u);
    break;
  case eParamRelDur:
    env_asr_set_rel_dur(env, v.u);
    break;
  case eParamAtkCurve:
    //    env_asr_set_atk_shape(env, float_to_fr32(v));
    env_asr_set_atk_shape(env, v.fr);
    break;
  case eParamRelCurve:
    env_asr_set_atk_shape(env, v.fr);
    break;
  case eParamHz1Smooth:
    filter_1p_fix16_set_hz(hz1Lp, v.fix);
    break;
  case eParamHz2Smooth:
    filter_1p_fix16_set_hz(hz2Lp, v.fix);
    break;
  case eParamPmSmooth:
    filter_1p_fr32_set_hz(pmLp, v.fix);
    break;
  case eParamWave1Smooth:
    filter_1p_fr32_set_hz(wave1Lp, v.fix);
    break;
  case eParamWave2Smooth:
    filter_1p_fr32_set_hz(wave2Lp, v.fix);
    break;
  case eParamAmp1Smooth:
    filter_1p_fr32_set_hz(amp1Lp, v.fix);
    break;
  case eParamAmp2Smooth:
    filter_1p_fr32_set_hz(amp2Lp, v.fix);
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
#ifdef ARCH_BFIN 
void module_process_frame(void) {
  calc_frame();
  out0 = frameVal;
  out1 = frameVal;
  out2 = frameVal;
  out3 = frameVal; 
}

static u8 ledstate = 0;
u8 module_update_leds(void) {
  return ledstate;
}

static u8 buts[4] = {0, 0, 0, 0};

void module_handle_button(const u16 state) {
  static pval v;
  static u8 b;
  
  // gate button
  b = (state & 0x100) > 0;
  if (buts[0] != b) {
    buts[0] = b;
    v.s = b;
    module_set_param(eParamGate, v);    
  }

  // hz1 button
  b = (state & 0x200) > 0;
  if (buts[1] != b) {
    buts[1] = b;
    v.fix = (b ? 330 : 220) << 16;
    set_hz1(v.fix);
  }

  // ratio2 button
  b = (state & 0x400) > 0;
  if (buts[2] != b) {
    buts[2] = b;
    v.fix = (b ? 0x28000 : 0x18000);
    set_ratio2(v.fix);
  }

  // pm button
  b = (state & 0x800) > 0;
  if (buts[3] != b) {
    buts[3] = b;
    v.fr = (b ? 0x7fffffff : 0);
    module_set_param(eParamPm, v);
  }

  ledstate = (u8)( (state>>8) & 0x3f);
  ledstate ^=0x3f;
}


#else //  non-bfin
void module_process_frame(const f32* in, f32* out) {
  u32 frame;
  u8 chan;
  for(frame=0; frame<BLOCKSIZE; frame++) {
    calc_frame();
    for(chan=0; chan<NUMCHANNELS; chan++) { // stereo interleaved
      // FIXME: use fract for output directly (portaudio setting?)
      *out++ = fr32_to_float(frameVal);
    }
  }
}
#endif
