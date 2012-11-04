/* sine.c
 * nullp
 * 
 * sine ( lookup ) oscillator
 */

// std
#include <math.h>
// libfixmath
#include "fix16.h"
#include "fract32.h"
// aleph-common
#include "audio.h"
#include "fix.h"
#include "fract32_emu.h"
#include "module.h"
// null
#include "types.h"

/*
  wavetable numerics:
  - use (signed) 16.16 for phase
  - constrain table size to 2^N
  - then we can get table index with phase >> (32 - N) -1)
    ( lose one bit from signed->unsigned) 
  - cast fractional part to fract32 (<<16 b/c unsigned->signed)
  - interpolate using fract32

 */

//--- wavetable
#define TAB_SIZE 512
#define TAB_SIZE_1 511
#define TAB_MAX16 0x1ff0000 // 511 * 0x10000
//----  parameters
#define PARAM_HZ  0
#define PARAM_AMP 1
// hz is fix16
#define HZ_MIN 1966080 // 30 << 16
#define HZ_MAX 524288000 // 8000 << 16


//-----------------------
//------ static variables
static fract32   tab[TAB_SIZE]; // wavetable

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
}

static void calc_frame(void) {
  static s16 x;
  static s16 xnext;
  static fract32 fx;
  static fract32 fxnext;
  x = (idx >> 16) % TAB_SIZE;
  xnext = (x + 1) % TAB_SIZE;

  // (unsigned LJ) -> (signed RJ)
  fxnext = (fract32)( (idx << 15) & 0x7fffffff );
  // invert
  fx = sub_fr1x32(0x7fffffff, fxnext);

  /////// TEST:
  //  fxnext = 0x3fffffff;
  //  fx = 0x80000000;
  
  // interpolate
  frameval = mult_fr1x32x32(amp, 
		   add_fr1x32(
			      mult_fr1x32x32(tab[x], fx),
			      mult_fr1x32x32(tab[xnext], fxnext)
			      )
		   );  
  
  // increment idx
  idx = fix16_add(idx, inc);
  while(idx > TAB_MAX16) { idx = fix16_sub(idx, TAB_MAX16); }
}

//----------------------
//----- external functions

void module_init(const u32 sr_arg) {
  f32 x = 0.f;
  f32 tabInc;
  u16 i;
  // init params
  sr = sr_arg;
  tabInc =  M_PI * 2.0 / (f32)TAB_SIZE;
  inc_1hz = fix16_from_float( (f32)TAB_SIZE / (f32)sr );
  set_hz( fix16_from_int(220) );
  amp = INT32_MAX >> 1;
  idx = 0;

  // init wavetable
  for(i=0; i<TAB_SIZE; i++) {
    tab[i] = float_to_fr32( sinf(x) );
    x += tabInc;
  }
}

void module_set_param(const u32 idx, const f32 val) {
  if(idx == 0) {
    set_hz(fix16_from_float(val));
  }
}

void module_callback(const f32* in, f32* out) {
  u32 frame;
  u8 chan;
  
  for(frame=0; frame<BLOCKSIZE; frame++) {
    calc_frame();
    for(chan=0; chan<NUMCHANNELS; chan++) { // stereo interleaved
      *out++ = fr32_to_float(frameval);
      // wire:
      //      *out++ = *in++;
    }
  }
}
