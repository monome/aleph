/* fsine.c
 * nullp
 * 
 * fast sine (IIR) test module
 */

#include <math.h>

#include "audio.h"
#include "module.h"
#include "fract32.h"

static f32 ip;         // initial phase
static f32 hz;         // frequency in hz
static f32 w;          // frequency in radians/sample
static f32 b;         // coefficient
static f32 z0;         // output sample history
static f32 z1;
static f32 z2;
static u32 sr = 44100;

//----------------------
//----- static functions

static void set_hz(const f32 hzArg) {
  hz = hzArg;
  if(hz < 30) hz = 30;
  if(hz > 8000) hz = 8000;
  w = hz * (f32)M_PI / (f32)sr;
  b = 2.f * cosf(w);
}

static void calc_frame(void) {
  z0 = b*z1 - z2;
  z2 = z1;
  z1 = z0;
}

//----------------------
//----- external functions

void module_init(const u32 sr_arg) {
  sr = sr_arg;
  ip = 3.f;
  set_hz(220.f);
}

void module_set_param(const u32 idx, const f32 val) {
  if(idx == 0) {
    set_hz(val);
  }
}

void module_callback(const f32* in, f32* out) {
  u32 frame;
  u8 chan;
  
  for(frame=0; frame<BLOCKSIZE; frame++) {
    calc_frame();
    for(chan=0; chan<NUMCHANNELS; chan++) { // stereo interleaved
      *out++ = z0;
    }
  }
}
