/* noise.h
   aleph-audio

   noise generators
*/

#include "types.h"


// linear congruential PRNG
// modulo is fixed for efficiency at 0xffffffff
// but offset, multiplier, and current value can be modulated at will.
// for maximal noisiness, best to discard the least significant bits of this output.
// for musical variation, could compute at arbitrary intervals and interpolate.
typedef struct _lcprng {
  // offset
  u32 a;
  // multiplier
  u32 c;
  // state
  u32 x;
} lcprng;

// initialize to reasonable values
extern void lcprng_reset(lcprng* o);
// set params
extern void lcprng_set_a(lcprng* o, s32 v);
extern void lcprng_set_c(lcprng* o, s32 v);
extern void lcprng_set_x(lcprng* o, s32 v);
// compute next
extern s32 lcprng_next(lcprng* o);