#ifndef _ALEPH_DSP_BIQUAD_H_
#define _ALEPH_DSP_BIQUAD_H_


#include "types.h"


typedef struct {
  fract32 a1;
  fract32 a2;
  fract32 b0;
  fract32 b1;
  fract32 b2;
  fract32 y_;
  fract32 y__;
  fract32 x_;
  fract32 x__;
} biquad;

void biquad_init (biquad *bq);

int biquad_next(biquad *bq, fract32 x);

void set_biquad_lpf (biquad *bq, float f, float q);

#define FR8_24_UNITY (1 << 24)

#define mult_fr8x24(a,b) (mult_fr1x32x32(a,b) << 7)
#define mult_fr7_24(a,b) (shl_fr1x32(mult_fr1x32x32(a,b), 7))
/* #define mult_fr15_16(a,b) (shl_fr1x32(mult_fr1x32x32(a,b), 15)) */

#define float_to_radix32(my_float,my_radix) ((fract32)((((float)my_float) * ((float) FR32_MAX)) / ((float)pow(2,my_radix))))

#define float_c8x24(my_float) float_to_radix32(my_float,7)

void set_biquad_coeffs_from_floats(biquad *bq, float a1, float a2, float b0, float b1, float b2);
#endif
