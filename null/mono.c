/* mono.c
 * nullp
 * 
 * monosynth module
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
#define SINE_TAB_SIZE 512
#define SINE_TAB_SIZE_1 511
//#define SINE_TAB_MAX16 0x1ff0000 // 511 * 0x10000
#define SINE_TAB_MAX16 0x1ffffff // (512 * 0x10000) - 1

//-------------------------
//----- extern vars (initialized here)
moduleData_t * moduleData; // module data

//-----------------------
//------ static variables
static fract32   sinetab[SINE_TAB_SIZE]; // wavetable
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
static fix16     hz2;            // base frequency (primary oscillator)
static fix16     ratio2;         // frequency ratio for secondary oscillator
static u8        track;         // pitch-tracking flag
static fract32   amp1;          // amplitude (primary)
static fract32   amp2;          // amplitude (secondary)

static env_asr*  env;           // ASR amplitude envelope 
static filter_1p_fix16* hz1Lp;  // 1plp smoother for hz1
static filter_1p_fix16* hz2Lp;  // 1plp smoother for hz2
static filter_1p_fix16* pmLp;  // 1plp smoother for pm
static filter_1p_fix16* wave1Lp;  // 1plp smoother for wave1
static filter_1p_fix16* wave2Lp;  // 1plp smoother for wave2
static filter_1p_fix16* amp1Lp;  // 1plp smoother for amp1
static filter_1p_fix16* amp2Lp;  // 1plp smoother for amp2


//-----------------------
//----- fwd declaration 
// from table.h
extern inline fract32 fixtable_lookup_idx(fract32* tab, u32 size, fix16 idx);
// from fix.h

//----------------------
//----- static functions

// set hz
static void set_hz1(const fix16 hzArg) {  
  hz1 = hzArg;
  if( hz1 < HZ_MIN ) hz1 = HZ_MIN;
  if( hz1 > HZ_MAX ) hz1 = HZ_MAX;
  inc1 = fix16_mul(hz1, ips);
  if(track) {
    inc2 = fix16_mul(fix16_mul(hz1, ratio2), ips);
  }
}

static void set_ratio2(const fix16 ratioArg) {
  ratio2 = ratioArg;
  if (ratio2 > RATIO_MAX) ratio2 = RATIO_MAX;
  if (ratio2 < RATIO_MIN) ratio2 = RATIO_MIN;
  inc2 = fix16_mul(fix16_mul(hz1, ratio2), ips);
  track = 1;
}

// frame calculation
static void calc_frame(void) {
  static u8 dum=0;
  // lookup osc2
  osc2 = fixtable_lookup_idx(sinetab, SINE_TAB_SIZE, idx2);
  
  /// use osc2 output as phase modulation, scaled to tablesize
  idx1Mod = fix16_add(
		      idx1,
		      fix16_mul(
				FRACT_FIX16(mult_fr1x32x32(osc2, pm)),
				SINE_TAB_MAX16)
		      );

  // wrap negative
  while (BSIGN(idx1Mod)) {
    //    idx1Mod = grjzkbadbl //............
    idx1Mod = fix16_add(idx1Mod, SINE_TAB_MAX16);
  }
  // wrap positive
  while(idx1Mod > SINE_TAB_MAX16) { 
    idx1Mod = fix16_sub(idx1Mod, SINE_TAB_MAX16); 
  }

  // lookup osc1
  osc1 = fixtable_lookup_idx(sinetab, SINE_TAB_SIZE, idx1Mod);

  // apply amplitudes and sum 
  frameVal = add_fr1x32(
			mult_fr1x32x32(osc1, amp1),
			mult_fr1x32x32(osc2, amp2)
			);

  // increment and apply envelope
  frameVal = mult_fr1x32x32(frameVal, env_asr_next(env));

  // increment and apply hz smoother
  if(!(hz1Lp->sync)) {
    set_hz1(filter_1p_fix16_next(hz1Lp));
  } 

  // increment idx and wrap
  idx1 = fix16_add(idx1, inc1);
  while(idx1 > SINE_TAB_MAX16) { 
    idx1 = fix16_sub(idx1, SINE_TAB_MAX16);
  }
  idx2 = fix16_add(idx2, inc2);
  while(idx2 > SINE_TAB_MAX16) { 
    idx2 = fix16_sub(idx2, SINE_TAB_MAX16); 
  }
}

//----------------------
//----- external functions

void module_init(const u32 sr_arg) {

  f32 x = 0.f;
  f32 tabInc;

  u16 i;

  // init module/param descriptor
  moduleData = (moduleData_t*)malloc(sizeof(moduleData_t));
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
  sr = sr_arg;
  track = 1;
  ips = fix16_from_float( (f32)SINE_TAB_SIZE / (f32)sr );
  amp1 = amp2 = INT32_MAX >> 1;
  ratio2 = fix16_from_int(2);
  set_hz1( fix16_from_int(220) );
  idx1 = idx2 = 0;

  // init wavetable
    //      fixtable_fill_harm(sinetab, SINE_TAB_SIZE, 4, 0.5f, 1);
  fixtable_fill_harm(sinetab, SINE_TAB_SIZE, 1, 1.f, 0);

  // allocate envelope
  env = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(env);
  env_asr_set_atk_shape(env, float_to_fr32(0.5));
  env_asr_set_rel_shape(env, float_to_fr32(0.5));

  // allocate 1pole hz smoother
  hz1Lp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init(hz1Lp, SAMPLERATE);
  filter_1p_fix16_set_hz(hz1Lp, 1.0);
  filter_1p_fix16_in(hz1Lp, hz1);

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
    //    set_hz(fix16_from_float(v));
    filter_1p_fix16_in(hz1Lp, fix16_from_float(v));
    break;
  case eParamRatio2:
    //    set_hz(fix16_from_float(v));
    //    filter_1p_fix16_in(hz1Lp, fix16_from_float(v));
    set_ratio2(fix16_from_float(v));
    break;
  case eParamWave1:
    break;
  case eParamWave2:
    break;
  case eParamPm:
    pm = float_to_fr32(v);
    // scale to [0, 0.5]
    pm >>= 1;
    break;
  case eParamAmp1:
    amp1 = float_to_fr32(v);
    break;
  case eParamAmp2:
    amp2 = float_to_fr32(v);
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

	//// DEBUG      
      /*
      if( (framecount < 256) && (chan == 0) && (*out != 0.0) ) { 
       	printf("\n%f,", *out);
	framecount++;
      }
      */
      
    }
  }
}
