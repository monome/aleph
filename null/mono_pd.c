/* mono_pd.c
 * nullp
 * 
 * a monosynth module using phase distortion synthesis (cz-1 style)
 */

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "fract32_emu.h"
#include "simple_string.h"
// null
#include "audio.h"
#include "env.h"
#include "filters.h"
#include "module.h"
#include "table.h"
#include "types.h"

/// DEBUG
static u32 framecount = 0;

///// inputs
enum params {
  eParamHz1,
  eParamHz2,
  eParamRatio2,
  eParamWave1,
  eParamGate,
  eParamAtkDur,
  eParamRelDur,
  eParamAtkCurve,
  eParamRelCurve,
  eParamHz1Smooth,
  eParamHz2Smooth,
  eParamWave1Smooth,
  
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
#define IDX_PHASE_LS 6 // left-shift to get full-scale fract32 from phase
//-------------------------
//----- extern vars (initialized here)
moduleData_t * moduleData; // module data

//-----------------------
//------ static variables
static fract32   tab1[WAVE_TAB_SIZE]; // wavetable1
static fract32   tab2[WAVE_TAB_SIZE]; // wavetable2

static u32       sr;            // sample rate
static fix16     ips16;        // index change per sample (at 1hz) in fix16
//static fract32     ips32;        // index change per sample (at 1hz) in fract32

static fix16     oscIdx;          // current phase (fractional idx) (primary)
static fract32   rPhase;          // current phase (unit) (reset pulse)

static fix16     oscInc;          // oscillator index increment per sample
static fract32    rInc;          // phase increment for reset pulse

static fract32   osc1;          // primary oscillator output
static fract32   frameVal;      // output value

static fix16     hz1;            // base frequency (primary oscillator)
static fix16     hz2;            // base frequency (phase reset)

static fix16     ratio2;         // frequency ratio for phase reset
static u8        track;         // pitch-tracking flag

static fract32   wave1;          // waveshape (primary)

static env_asr*  env;           // ASR amplitude envelope
 
static filter_1p_fix16* hz1Lp;  // 1plp smoother for hz1
static filter_1p_fix16* hz2Lp;  // 1plp smoother for hz2
static filter_1p_fr32* wave1Lp; // 1plp smoother for wave1


//-----------------------
//----- fwd declaration 
// from table.h
extern inline fract32 fixtable_lookup_idx(fract32* tab, u32 size, fix16 idx);
// from fix.h

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
		    mult_fr1x32x32(fixtable_lookup_idx(tab1, WAVE_TAB_SIZE, idx), waveInv),
		    mult_fr1x32x32(fixtable_lookup_idx(tab2, WAVE_TAB_SIZE, idx), wave)
		     );
}

// frame calculation
static void calc_frame(void) {
  ////// FIXME: debug
  fract32 phaseMul;

  // ----- smoothers:
  // wave1
  wave1 = filter_1p_fr32_next(wave1Lp);

  // lookup osc1
  //  osc1 = fixtable_lookup_idx(tab1, WAVE_TAB_SIZE, idx1Mod);
  osc1 = lookup_wave(oscIdx, wave1);

  phaseMul = sub_fr1x32(INT32_MAX, oscIdx << IDX_PHASE_LS);

  /// ... multiply osc output by inverted phase
    frameVal = mult_fr1x32x32( osc1, phaseMul );

  // increment and apply envelope
  frameVal = mult_fr1x32x32(frameVal, env_asr_next(env));

  // increment and apply hz smoothers
  if(!(hz1Lp->sync)) {
    oscInc = fix16_mul(filter_1p_fix16_next(hz1Lp), ips16);
  } 
  if(!(hz2Lp->sync)) {
    //// = fix16_mul(filter_1p_fix16_next(hz2Lp), ips);
    //// ...
    rInc = FIX16_FRACT_TRUNC(fix16_mul(filter_1p_fix16_next(hz2Lp), ips16));
  } 

  // increment idx and wrap
  oscIdx = fix16_add(oscIdx, oscInc);
  while(oscIdx > WAVE_TAB_MAX16) { 
    oscIdx = fix16_sub(oscIdx, WAVE_TAB_MAX16);
  }

  // increment reset phase
  rPhase = add_fr1x32(rPhase, rInc);
  // check for overflow
  if (rPhase < 0) { 
    // reset primary oscillator
    oscIdx = 0;
    rPhase += INT32_MAX;
  }

}

//----------------------
//----- external functions

void module_init(const u32 sr_arg) {

  // init module/param descriptor
  moduleData = (moduleData_t*)malloc(sizeof(moduleData_t));
  moduleData->numParams = eParamNumParams;
  moduleData->paramDesc = (ParamDesc*)malloc(eParamNumParams * sizeof(ParamDesc));
  moduleData->paramData = (ParamData*)malloc(eParamNumParams * sizeof(ParamData));

  strcpy(moduleData->paramDesc[eParamHz1].label, "osc 1 hz");
  strcpy(moduleData->paramDesc[eParamHz2].label, "osc 2 hz");
  strcpy(moduleData->paramDesc[eParamRatio2].label, "osc 2 ratio");
  strcpy(moduleData->paramDesc[eParamWave1].label, "waveshape 1");
  strcpy(moduleData->paramDesc[eParamGate].label, "gate");
  strcpy(moduleData->paramDesc[eParamAtkDur].label, "amp env attack");
  strcpy(moduleData->paramDesc[eParamRelDur].label, "amp env release");
  strcpy(moduleData->paramDesc[eParamAtkCurve].label, "amp env atk curve");
  strcpy(moduleData->paramDesc[eParamRelCurve].label, "amp env rel curve");
  strcpy(moduleData->paramDesc[eParamHz1Smooth].label, "hz 1 smoothing");
  strcpy(moduleData->paramDesc[eParamHz2Smooth].label, "hz 2 smoothing");
  strcpy(moduleData->paramDesc[eParamWave1Smooth].label, "wave 1 smoothing");

  // init params
  sr = sr_arg;
  track = 1;
  ips16 = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );
  //  ips32 = float_to_fr32( 1.f / (f32)sr );
  
  ratio2 = fix16_from_int(2);
  hz1 = fix16_from_int(220);
  oscIdx = rPhase = 0;

  // init wavetables
  fixtable_fill_harm(tab1, WAVE_TAB_SIZE, 1, 1.f, 0);
  fixtable_fill_harm(tab2, WAVE_TAB_SIZE, 5, 0.5, 1);

  // allocate envelope
  env = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(env);
  env_asr_set_atk_shape(env, float_to_fr32(0.5));
  env_asr_set_rel_shape(env, float_to_fr32(0.5));

  // allocate smoothers
  hz1Lp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( hz1Lp, SAMPLERATE, 10.0, hz1 );
  hz2Lp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( hz2Lp, SAMPLERATE, 10.0, hz2 );

  wave1Lp = (filter_1p_fr32*)malloc(sizeof(filter_1p_fr32));
  filter_1p_fr32_init( wave1Lp, SAMPLERATE, 10.0, wave1 );

  /// DEBUG
  printf("\n\n module init debug \n\n");
}

// de-init
void module_deinit(void) {
  free(env);
  free(hz1Lp);
}

// set parameter by value
void module_set_param(u32 idx, f32 v) {
  switch(idx) {
  case eParamHz1:
    set_hz1(fix16_from_float(v));
    break;
  case eParamRatio2:
    set_ratio2(fix16_from_float(v));
    break;
  case eParamHz2:
    set_hz2(fix16_from_float(v));
    break;
  case eParamWave1:
    filter_1p_fr32_in(wave1Lp, float_to_fr32(v));
    break;
  case eParamGate:
    env_asr_set_gate(env, v > 0.f);
    break;
  case eParamAtkDur:
    env_asr_set_atk_dur(env, (u32)v);
    break;
  case eParamRelDur:
    env_asr_set_rel_dur(env, (u32)v);
    break;
  case eParamAtkCurve:
    env_asr_set_atk_shape(env, float_to_fr32(v));
    break;
  case eParamRelCurve:
    env_asr_set_atk_shape(env, float_to_fr32(v));
    break;
  case eParamHz1Smooth:
    filter_1p_fix16_set_hz(hz1Lp, fix16_from_float(v));
    break;
  case eParamHz2Smooth:
    filter_1p_fix16_set_hz(hz2Lp, fix16_from_float(v));
    break;
  case eParamWave1Smooth:
    filter_1p_fr32_set_hz(wave1Lp, fix16_from_float(v));
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
