#include "fix16_fract.h"
// multiply 2 numbers in 16.16 base making use of blackfin fract
// primitives
fract32 fix16_mul_fract(fract32 a, fract32 b) {
  char a_radix = norm_fr1x32(a);
  char b_radix = norm_fr1x32(b);
  return shl_fr1x32(mult_fr1x32x32(shl_fr1x32(a, a_radix), shl_fr1x32(b, b_radix)),
                               15 - a_radix - b_radix);
}

