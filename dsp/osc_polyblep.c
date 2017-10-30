#include "osc_polyblep.h"
#include "fract_math.h"
#include "fix.h"
#include "ricks_tricks.h"
#include "biquad.h"
#include "libfixmath/fix16_fract.h"

static inline fix16 polyblep (fract32 p, fract32 dp) {
  fix16 dp_inv = FR32_MAX / shr_fr1x32(dp, 16);

  fix16 p_by_dp = fix16_mul_fract(dp_inv, shr_fr1x32(p, 15));
  fix16 p_plus_one_by_dp = dp_inv + p_by_dp;
  fix16 one_minus_p_by_dp = dp_inv - p_by_dp;

  if (p < add_fr1x32(dp, FR32_MIN))
    return shl_fr1x32(p_plus_one_by_dp, 1)
      - fix16_mul_fract_radix(p_plus_one_by_dp, p_plus_one_by_dp, 15)
      - 0x10000;

  if (p > sub_fr1x32(FR32_MAX, dp))
    return negate_fr1x32(shl_fr1x32(one_minus_p_by_dp, 1)
			 - fix16_mul_fract_radix(one_minus_p_by_dp, one_minus_p_by_dp, 15)
			 - 0x10000);
  return 0;
}

fract16 square_polyblep (fract32 p, fract32 dp) {
  fix16 square_raw = 0xFFFF;
  if (p < 0)
    square_raw *= -1;
  fix16 square_polyblep = add_fr1x32(square_raw, polyblep(p+FR32_MAX, dp));
  square_polyblep = sub_fr1x32(square_polyblep, polyblep(p, dp));
  return (fract16) shr_fr1x32(square_polyblep, 1);
}

fract16 saw_polyblep (fract32 p, fract32 dp) {
  return shr_fr1x32(sub_fr1x32(shr_fr1x32(p, 15), polyblep(p, dp)), 1);
}

fract16 sine_polyblep (fract32 phase) {
  fract16 phase16;
  phase -= FR32_MAX / 2;
  if (phase > (1 << 30) || phase < (-1 << 30)) {
    phase16 = FR16_MIN - trunc_fr1x32(phase);
    return sub_fr1x16(shl_fr1x16(multr_fr1x16(phase16, phase16), 2),
		      FR16_MAX);
  }
  else if (phase < (1 << 30) || phase <= (-1 << 30)) {
    phase16 = trunc_fr1x32(phase);
    return sub_fr1x16(FR16_MAX,
		      shl_fr1x16(multr_fr1x16( phase16, phase16), 2));
  }
  else
    return 0;
}

fract16 triangle_polyblep (fract32 phase) {
  fract16 phase16;
  phase -= FR32_MAX / 2;
  if (phase > (1 << 30) || phase < (-1 << 30)) {
    phase16 = FR16_MIN - trunc_fr1x32(phase);
    return sub_fr1x16 (shl_fr1x16(abs_fr1x16(phase16), 1),
		       FR16_MAX);
  }
  else if (phase < (1 << 30) || phase <= (-1 << 30)) {
    phase16 = trunc_fr1x32(phase);
    return sub_fr1x32(FR16_MAX,
		      shl_fr1x16(abs_fr1x16(phase16), 1));
  }
  else
    return 0;

}

