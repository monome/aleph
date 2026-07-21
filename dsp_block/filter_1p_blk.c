/* filter_1p_blk.c
 * aleph dsp_block
 *
 * Block-rate 1-pole slew. UI/bees still send per-sample coeffs; set_slew
 * converts c_s -> c_s^N so one prepare() approximates N sample steps.
 */

#include "filter_1p_blk.h"

#ifdef ARCH_BFIN
#include "fract_math.h"
#else
#include "fract32_emu.h"
#endif

#define FR32_COMP_THRESH 0x4000

static inline u8 fr32_compare(fract32 a, fract32 b) {
  return (abs_fr1x32(sub_fr1x32(a, b)) < FR32_COMP_THRESH);
}

/* fract32 power: base^exp with identity ~ FR32_MAX (1.0 in Q1.31). */
static fract32 fr32_pow_u16(fract32 base, u16 exp) {
  fract32 acc;
  fract32 b;
  u16 n;

  if(exp == 0) {
    return FR32_MAX;
  }
  if(base <= 0) {
    return 0;
  }
  if(base >= FR32_MAX) {
    return FR32_MAX;
  }

  acc = FR32_MAX;
  b = base;
  n = exp;
  while(n > 0) {
    if(n & 1u) {
      acc = mult_fr1x32x32(acc, b);
    }
    n >>= 1;
    if(n) {
      b = mult_fr1x32x32(b, b);
    }
  }
  return acc;
}

void filter_1p_lo_blk_init(filter_1p_lo_blk *f, fract32 in, u16 blocksize) {
  f->y = in;
  f->x = in;
  f->c = 0;
  f->blocksize = (blocksize == 0) ? 1 : blocksize;
}

void filter_1p_lo_blk_set_slew(filter_1p_lo_blk *f, fract32 sample_slew) {
  u16 n = f->blocksize;
  if(n <= 1) {
    f->c = sample_slew;
    return;
  }
  f->c = fr32_pow_u16(sample_slew, n);
}

void filter_1p_lo_blk_in(filter_1p_lo_blk *f, fract32 val) {
  f->x = val;
}

void filter_1p_lo_blk_prepare(filter_1p_lo_blk *f) {
  f->y = add_fr1x32(f->x, mult_fr1x32x32(f->c, sub_fr1x32(f->y, f->x)));
}

fract32 filter_1p_lo_blk_next(filter_1p_lo_blk *f) {
  return f->y;
}

u8 filter_1p_lo_blk_sync(filter_1p_lo_blk *f) {
  if(f->x == f->y) {
    return 1;
  }
  if(fr32_compare(f->x, f->y)) {
    f->y = f->x;
    return 0;
  }
  return 0;
}
