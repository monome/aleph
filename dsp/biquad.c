#include <fract_math.h>
#include <math.h>

#include "fix.h"
#include "types.h"

#include "biquad.h"

#include "ricks_tricks.h"

void biquad_init (biquad *bq) {
  bq->a1 =0;
  bq->a2 =0;
  bq->b0 =0;
  bq->b1 =0;
  bq->b2 =0;

  bq->x_ = 0;
  bq->x__ = 0;
  bq->y_ = 0;
  bq->y__ = 0;

}

int biquad_next(biquad *bq,
		     fract32 x) {
  fract32 ret = (mult_fr8x24(bq->b0,x) + mult_fr8x24(bq->b1, bq->x_) + mult_fr8x24(bq->b2,bq->x__))
    - (mult_fr8x24(bq->a1, bq->y_) + mult_fr8x24(bq->a2, bq->y__));
  bq->y__ = bq->y_;
  bq->y_ = ret;
  bq->x__ = bq->x_;
  bq->x_ = x;
  return ret;
}

void set_biquad_coeffs_from_floats(biquad *bq, float a1, float a2, float b0, float b1, float b2) {
  bq->a1 = float_c8x24(a1);
  bq->a2 = float_c8x24(a2);
  bq->b0 = float_c8x24(b0);
  bq->b1 = float_c8x24(b1);
  bq->b2 = float_c8x24(b2);
}

void set_biquad_lpf(biquad *bq, float f, float q) {
  float a0, a1, a2, b0, b1, b2, omega, alpha;
  omega = (2 * 3.1415 * f) / 48000.0;
  alpha = sin(omega) / (2.0 * q);

  b0 =  (1 - cos(omega)) / 2;
  b1 =   1 - cos(omega);
  b2 =  (1 - cos(omega)) / 2;
  a0 =   1 + alpha;
  a1 =  -2 * cos(omega);
  a2 =   1 - alpha;

  bq->a1 = float_c8x24(a1 / a0);
  bq->a2 = float_c8x24(a2 / a0);
  bq->b0 = float_c8x24(b0 / a0);
  bq->b1 = float_c8x24(b1 / a0);
  bq->b2 = float_c8x24(b2 / a0);
}

