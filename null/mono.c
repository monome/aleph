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

enum params {
  eParamHz1,
  eParamHz2,
  eParamFm,
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
  eParamNumParams
};

//----  parameters
/*
#define PARAM_HZ1  0
#define PARAM_HZ2  0
#define P
#define PARAM_AMP 1
#define PARAM_GATE 2
#define PARAM_ATK_DUR 3
#define PARAM_REL_DUR 4
#define PARAM_ATK_CURVE 5
#define PARAM_REL_CURVE 6
#define PARAM_HZ_SMOOTH 7
#define NUM_PARAMS 8
*/

// hz is fix16
#define HZ_MIN 1966080 // 30 << 16
#define HZ_MAX 524288000 // 8000 << 16

//--- wavetable
#define SINE_TAB_SIZE 512
#define SINE_TAB_SIZE_1 511
//#define SINE_TAB_MAX16 0x1ff0000 // 511 * 0x10000
#define SINE_TAB_MAX16 0x1ffffff // (512 * 0x10000) - 1

//-----------------------
//----- fwd declaration 
// from table.h
extern inline fract32 fixtable_lookup_idx(fract32* tab, u32 size, fix16 idx);

//-------------------------
//----- extern vars (initialized here)
moduleData_t * moduleData; // module data

//-----------------------
//------ static variables

static fract32   sinetab[SINE_TAB_SIZE]; // wavetable
static fix16     idx;        // current phase (fractional idx)
static fix16     inc_1hz;      // idx change at 1hz
static fix16     inc;          // idx change at current frequey
static fract32   frameval;     // output value
static fract32   amp;          // amplitude
static u32       sr;           // sample rate
static fix16     hz;           // frequency
static env_asr*  env;          // ASR envelope 
static filter_1p_fix16* hz_1p;       // 1-pole lowpass for smoothing hz

//----------------------
//----- static functions

// set hz
static void set_hz(const fix16 hzArg) {  
  hz = hzArg;
  if( hz < HZ_MIN ) hz = HZ_MIN;
  if( hz > HZ_MAX ) hz = HZ_MAX;
  inc = fix16_mul(hz, inc_1hz);
  //  idx = 0;
}

// frame calculation
static void calc_frame(void) {
  // lookup
  frameval = fixtable_lookup_idx(sinetab, SINE_TAB_SIZE, idx);
  // increment and apply envelope
  frameval = mult_fr1x32x32(frameval, env_asr_next(env));
  // apply amplitude 
  frameval = mult_fr1x32x32(frameval, amp);

  // increment and apply hz smoother
  if(!(hz_1p->sync)) {
    set_hz(filter_1p_fix16_next(hz_1p));
  } 

  // increment idx and wrap
  idx = fix16_add(idx, inc);
  while(idx > SINE_TAB_MAX16) { idx = fix16_sub(idx, SINE_TAB_MAX16); }
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

  //  moduleData->paramDesc[0].label = "osc 1 hz";
  //  moduleData->paramDesc[0].label = "osc 2 hz";
  strcpy(moduleData->paramDesc[0].label, "osc 1 hz");
  // init params
  sr = sr_arg;
  
  inc_1hz = fix16_from_float( (f32)SINE_TAB_SIZE / (f32)sr );
  amp = INT32_MAX >> 1;
  set_hz( fix16_from_int(220) );
  idx = 0;

  // init wavetable
    //      fixtable_fill_harm(sinetab, SINE_TAB_SIZE, 4, 0.5f, 1);
  fixtable_fill_harm(sinetab, SINE_TAB_SIZE, 1, 1.f, 0);

  // allocate envelope
  env = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(env);
  env_asr_set_atk_shape(env, float_to_fr32(0.5));
  env_asr_set_rel_shape(env, float_to_fr32(0.5));

  // allocate 1pole hz smoother
  hz_1p = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init(hz_1p, SAMPLERATE);
  filter_1p_fix16_in(hz_1p, 300.0);
  filter_1p_fix16_set_hz(hz_1p, 1.0);

  /// DEBUG
  printf("\n\n module init debug \n\n");
}

// de-init
void module_deinit(void) {
  free(env);
  free(hz_1p);
}

// set parameter by value
void module_set_param(u32 idx, f32 v) {
  switch(idx) {
  case eParamHz1:
    //    set_hz(fix16_from_float(v));
    filter_1p_fix16_in(hz_1p, fix16_from_float(v));
    break;
  case eParamWave1:
    break;
  case eParamWave2:
    break;
  case eParamFm:
    break;
  case eParamAmp1:
    amp = float_to_fr32(v);
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
    filter_1p_fix16_set_hz(hz_1p, fix16_from_float(v));
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
      *out++ = fr32_to_float(frameval);

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
