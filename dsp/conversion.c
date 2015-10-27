#include "conversion.h"

#include "fix.h"
#include "module.h"


/// convert fix16 seconds to u32 frames, truncating

// convert seconds in 16.16 to samples in 32.32
void sec_to_frames_fract(fix16* time, fix32* samps) {
  static float fSamps;
  // FIXME: using float
  fSamps = (float)SAMPLERATE * fix16_to_float(*time);
  samps->i = (int)fSamps;
  samps->fr = float_to_fr32(fSamps - samps->i);
}

u32 sec_to_frames_trunc(fix16 sec) {
  // FIXME: using float...
  return (u32)((float)SAMPLERATE * fix16_to_float(sec));
  /*
  u32 res;

  // whole seconds
  if(sec > 0xffff) {
     res = (u32)( (sec >> 16 ) * SAMPLERATE );
  } else {
    res = 0;
  }
  // fractional seconds
  // FIXME (?): assuming sample rate <= 65k, not a portable assumption
  // FIXME: should probly round instead
  res += (u32)(fix16_mul(sec & 0xffff, SAMPLERATE << 16) >> 16);
  return res;
  */

}


/*
// multiply fix16 by (truncated) fract32
// non-saturating, unfortunately
static inline fract32 mul_fr32_fix16(fract32 fr, fix16 fix) {
  return shl_fr1x32(fix16_mul(trunc_fr1x32(fr), fix), 15);
}

*/
