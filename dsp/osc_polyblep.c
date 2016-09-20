#include "osc_polyblep.h"
#include "fract_math.h"
#include "fix.h"
#include "ricks_tricks.h"
#include "biquad.h"

fract32 polyblep (fract32 p, fract32 dp) {
  fract32 dp_inv = FR32_MAX / shr_fr1x32(dp, 16);

  fract32 p_by_dp = fix16_mul(dp_inv, shr_fr1x32(p, 15));
  fract32 p_plus_one_by_dp = dp_inv + p_by_dp;
  fract32 one_minus_p_by_dp = dp_inv - p_by_dp;

  if (p < add_fr1x32(dp, FR32_MIN))
    return shl_fr1x32(p_plus_one_by_dp, 1)
      - fix16_mul(p_plus_one_by_dp, p_plus_one_by_dp)
      - 0x10000;

  if (p > sub_fr1x32(FR32_MAX, dp))
    return negate_fr1x32(shl_fr1x32(one_minus_p_by_dp, 1)
			 - fix16_mul(one_minus_p_by_dp, one_minus_p_by_dp)
			 - 0x10000);
  return 0;
}

fract32 square_polyblep (fract32 p, fract32 dp) {
  fix16 square_raw = 0x10000;
  if (p < 0)
    square_raw *= -1;
  fix16 square_polyblep = add_fr1x32(square_raw, polyblep(p+FR32_MAX, dp));
  square_polyblep = sub_fr1x32(square_polyblep, polyblep(p, dp));
  return square_polyblep;
}

fract32 saw_polyblep (fract32 p, fract32 dp) {
  return sub_fr1x32(shr_fr1x32(p, 15), polyblep(p, dp));
}
