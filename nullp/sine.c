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

// table size
#define SINETABSIZE 512
#define SINETABSIZE_1 511
// parameters
#define PARAM_HZ  0
#define PARAM_AMP 1

//-----------------------
//------ static variables

static fract32   sinetab[SINETABSIZE]; // wavetable
static fract32   phase;        // current phase
static fract32   rho_1hz;      // idx change at 1hz
static fract32   rho;          // idx change at current frequency
static fract32   frameval;          // output value
static fract32   amp;          // amplitude

static u32       sr;           // sample rate
static fix16     hz;           // frequency


//----------------------
//----- static functions

static void set_hz(const fix16 hzArg) {  
  
  hz = hzArg;

  if( hz < (30 << 16)   ) { hz = 30; }
  if( hz > (8000 << 16) ) { hz = 8000; }

  //// FIXME: might need to add some double-width conversion "intrinsics"
  /// need 16.16 conversion from fract32 !
  //  rho = (s64)( (s64)(rho_1hz) * (s64)(hz) ) >> 32 >> 16 
  //  rho = mult_fr1x32x32(rho_1hz, hz);
  rho = fix16_mul(hz, rho_1hz >> 16);
}

static void calc_frame(void) {
  static u16 x;
  static u16 xnext;
  static fract32 fx;
  static fract32 fxnext;
  
  x = FIX16_TO_U16(phase);
  xnext = x + 1;
  // get the fractional part as fract32 (lshift 16)
  // lose one bit from unsigned->signed
  fxnext = ((fract32)(phase & 0xffff)) << 15;
  // invert
  fx = sub_fr1x32(0x7fffffff, fxnext);
  // wrap
  if ( xnext > SINETABSIZE_1) {
    xnext -= SINETABSIZE_1;
  }
  // interpolate
  frameval = mult_fr1x32x32(amp, 
		   add_fr1x32(
			      mult_fr1x32x32(sinetab [x], fx),
			      mult_fr1x32x32(sinetab[xnext], fxnext)
			      )
		   );

  // increment phase
  phase = add_fr1x32(phase, rho);
}

//----------------------
//----- external functions

void module_init(const u32 sr_arg) {
  f32 x = 0.f;
  u16 i;
  // init params
  sr = sr_arg;
  rho_1hz = float_to_fr32( (f32)SINETABSIZE / (f32)sr );
  set_hz( fix16_from_int(220) );
  amp = INT32_MAX >> 2;
  // init wavetable
  for(i=0; i<SINETABSIZE; i++) {
    sinetab[i] = sinf(x);
    x += rho;
  }
}

void module_set_param(const u32 idx, const f32 val) {
  // TODO
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
      *out++ = frameval;
      // wire:
      //      *out++ = *in++;
    }
  }
}
