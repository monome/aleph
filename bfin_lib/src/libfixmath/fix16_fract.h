#ifndef __fix16_fract_h__
#define __fix16_fract_h__

/* #include "types.h" */
#include "fix.h"
#include "fract_math.h"


static inline fix16 fix16_mul_fract(fix16 a, fix16 b) {
  u8 a_radix = norm_fr1x32(a);
  u8 b_radix = norm_fr1x32(b);
  return shl_fr1x32(mult_fr1x32x32(shl_fr1x32(a, a_radix),
				   shl_fr1x32(b, b_radix)),
		    15 - a_radix - b_radix);
}

static inline fix16 fix16_mul_fract_radix(fix16 a, fix16 b, u8 r) {
  return shl_fr1x32(mult_fr1x32x32(shl_fr1x32(a, r),
				   shl_fr1x32(b, r)),
		    15 - r - r);
}
#endif
