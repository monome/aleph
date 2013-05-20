#include "conversion.h"

#include "fix.h"
#include "module.h"

/* // bfin */
/* #ifdef ARCH_BFIN */
/* #include "bfin_core.h" */
/* #include "fract_math.h" */
/* #include <fract2float_conv.h> */
/* #else */
/* // linux */
/* #include "fract32_emu.h" */
/* #include "audio.h" */
/* #endif */

/// convert fix16 seconds to u32 frames, truncating
extern u32 seconds_to_frames(fix16 sec) {
  u32 res;
  // whole seconds
  res = (u32)FIX16_TO_U16( BIT_ABS(sec) );
  res *= SAMPLERATE;
  // fractional seconds
  // FIXME (?): assuming sample rate <= 65k, not a portable assumption
  // FIXME: should probly round instead
  res += (u32)FIX16_TO_U16( fix16_mul(sec & 0xffff, fix16_from_int(SAMPLERATE)) );
  return res;
}


