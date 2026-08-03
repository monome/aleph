/* peak_meter_blk.c
 * aleph dsp_block
 *
 * Absolute peak-hold; block-rate decay toward 0.
 */

#include "peak_meter_blk.h"

#ifdef ARCH_BFIN
#include "fract_math.h"
#else
#include "fract32_emu.h"
#endif

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

void peak_meter_blk_init(peak_meter_blk *m, u16 blocksize) {
  m->peak = 0;
  m->blocksize = (blocksize == 0) ? 1 : blocksize;
  peak_meter_blk_set_decay(m, PEAK_METER_BLK_DECAY_DEFAULT);
}

void peak_meter_blk_set_decay(peak_meter_blk *m, fract32 sample_decay) {
  u16 n = m->blocksize;
  if(n <= 1) {
    m->decay = sample_decay;
    return;
  }
  m->decay = fr32_pow_u16(sample_decay, n);
}

void peak_meter_blk_prepare(peak_meter_blk *m) {
  m->peak = mult_fr1x32x32(m->decay, m->peak);
}

void peak_meter_blk_next(peak_meter_blk *m, fract32 in) {
  fract32 a = abs_fr1x32(in);
  if(a > m->peak) {
    m->peak = a;
  }
}

fract32 peak_meter_blk_read(peak_meter_blk *m) {
  return m->peak;
}
