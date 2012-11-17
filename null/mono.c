/* mono.c
 * nullp
 * 
 * monosynth module
 */

// std
#include <math.h>
// (testing)
#include <stdlib.h>

// aleph-common
#include "fix.h"
#include "fract32_emu.h"
// null
#include "audio.h"
#include "env.h"
#include "module.h"
#include "table.h"
#include "types.h"


/// DEBUG
static u32 framecount = 0;


//----  parameters
#define PARAM_HZ  0
#define PARAM_AMP 1
#define PARAM_GATE 2
#define PARAM_ATK_DUR 3
#define PARAM_REL_DUR 4
#define PARAM_ATK_CURVE 5
#define PARAM_REL_CURVE 6
#define NUM_PARAMS 7

// hz is fix16
#define HZ_MIN 1966080 // 30 << 16
#define HZ_MAX 524288000 // 8000 << 16

/*
  wavetable numerics:
  - use (signed) 16.16 for phase
  - constrain table size to 2^N
  - then we can get table index with phase >> (32 - N) -1)
    ( lose one bit from signed->unsigned) 
  - cast fractional part to fract32 (<<15 b/c unsigned->signed)
  - interpolate using fract32
 */

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
static env_asr* env; // envelope

static fract32   sinetab[SINE_TAB_SIZE]; // wavetable

static fix16     idx;        // current phase (fractional idx)
static fix16     inc_1hz;      // idx change at 1hz
static fix16     inc;          // idx change at current frequency

static fract32   frameval;     // output value
static fract32   amp;          // amplitude

static u32       sr;           // sample rate
static fix16     hz;           // frequency


//----------------------
//----- static functions

static void set_hz(const fix16 hzArg) {  
  hz = hzArg;
  if( hz < HZ_MIN ) hz = HZ_MIN;
  if( hz > HZ_MAX ) hz = HZ_MAX;
  inc = fix16_mul(hz, inc_1hz);
  //  idx = 0;
}

/// 

static void calc_frame(void) {
  // lookup
  frameval = fixtable_lookup_idx(sinetab, SINE_TAB_SIZE, idx);
  // apply envelope
  frameval = mult_fr1x32x32(frameval, env_asr_next(env));
  // apply amplitude 
  frameval = mult_fr1x32x32(frameval, amp);
  // increment idx and wrap
  idx = fix16_add(idx, inc);
  while(idx > SINE_TAB_MAX16) { idx = fix16_sub(idx, SINE_TAB_MAX16); }
  ///// DEBUG
  // print index
  /*
  if( (framecount < 512) ) {
       	printf("0x%08x , ", idx);
	framecount++;
      }
  */
}

//----------------------
//----- external functions

void module_init(const u32 sr_arg) {

  f32 x = 0.f;
  f32 tabInc;

  u16 i;

  // init module/param descriptor
  moduleData = (moduleData_t*)malloc(sizeof(moduleData_t));
  moduleData->numParams = NUM_PARAMS;
  moduleData->paramDesc = (ParamDesc*)malloc(NUM_PARAMS * sizeof(ParamDesc));
  moduleData->paramData = (ParamData*)malloc(NUM_PARAMS * sizeof(ParamData));

  // init params
  sr = sr_arg;
  
  inc_1hz = fix16_from_float( (f32)SINE_TAB_SIZE / (f32)sr );
  amp = INT32_MAX >> 1;
  set_hz( fix16_from_int(220) );
  idx = 0;

  
  // init wavetable
  
  /*
    tabInc =  M_PI * 2.0 / (f32)SINE_TAB_SIZE;
    for(i=0; i<SINE_TAB_SIZE; i++) {
    sinetab[i] = float_to_fr32( sinf(x) );
    x += tabInc;
    }
 */
  //      fixtable_fill_harm(sinetab, SINE_TAB_SIZE, 4, 0.5f, 1);
  fixtable_fill_harm(sinetab, SINE_TAB_SIZE, 1, 1.f, 0);


  // allocate envelope
  env = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(env);
  env_asr_set_atk_shape(env, float_to_fr32(0.5));
  env_asr_set_rel_shape(env, float_to_fr32(0.5));


  /// DEBUG
  printf("\n\n module init debug \n\n");
}

// de-init
void module_deinit(void) {
  free(env);
}

// set parameter by value
void module_set_param(u32 idx, f32 v) {
  switch(idx) {
  case PARAM_HZ:
    set_hz(fix16_from_float(v));
    break;
  case PARAM_AMP:
    amp = float_to_fr32(v);
    break;
  case PARAM_GATE:
    env_asr_set_gate(env, v > 0.f);
    break;
  case PARAM_ATK_DUR:
    env_asr_set_atk_dur(env, (u32)v);
    break;
  case PARAM_REL_DUR:
    env_asr_set_rel_dur(env, (u32)v);
    break;
  case PARAM_ATK_CURVE:
    env_asr_set_atk_shape(env, float_to_fr32(v));
    break;
  case PARAM_REL_CURVE:
    env_asr_set_atk_shape(env, float_to_fr32(v));
    break;

  }
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
      
      if( (framecount < 256) && (chan == 0) && (*out != 0.0) ) { 
       	printf("\n%f,", *out);
	framecount++;
      }
      
    }
  }
}
