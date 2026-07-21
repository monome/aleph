/* filter_bp_blk.h
 * aleph dsp_block
 *
 * Block-rate bandpass (1-pole HP then 1-pole LP). Classic sibling types
 * live in dsp/ricks_tricks.h (hpf / lpf / bpf).
 *
 * prepare() computes alphas once (integer divides); next() uses stored
 * alphas only.
 */

#ifndef _ALEPH_DSP_BLOCK_FILTER_BP_BLK_H_
#define _ALEPH_DSP_BLOCK_FILTER_BP_BLK_H_

#include "fix.h"
#include "fract_math.h"
#include "types.h"

typedef struct {
  fract32 lastIn;
  fract32 lastOut;
} filter_hp_blk;

typedef struct {
  fract32 lastOut;
} filter_lp_blk;

typedef struct {
  filter_hp_blk hp;
  filter_lp_blk lp;
  fract32 hp_alpha;
  fract32 lp_alpha;
} filter_bp_blk;

#define FILTER_BP_BLK_SR 48000
#define filter_bp_blk_hzToDimensionless(hz)                                    \
  ((fract32)((fract32)(hz) * (FR32_MAX / FILTER_BP_BLK_SR)))

#define FILTER_BP_BLK_TWO_PI_16_16 411775

static inline fract32 filter_bp_blk_one_over_x_16_16(fract32 x_16_16) {
  return shl_fr1x32(FR32_MAX / x_16_16, 16);
}

static inline fract32 filter_bp_blk_hpf_freq_calc(fract32 freq) {
  return filter_bp_blk_one_over_x_16_16(
      add_fr1x32(mult_fr1x32x32(FILTER_BP_BLK_TWO_PI_16_16, freq), 1 << 16));
}

static inline fract32 filter_bp_blk_lpf_freq_calc(fract32 freq) {
  fract32 temp = mult_fr1x32x32(FILTER_BP_BLK_TWO_PI_16_16, freq);
  return ((temp << 12) / ((1 << 16) + temp)) << 19;
}

void filter_hp_blk_init(filter_hp_blk *f);
void filter_lp_blk_init(filter_lp_blk *f);
void filter_bp_blk_init(filter_bp_blk *f);

/* alpha is an explicit parameter — no divide inside */
fract32 filter_hp_blk_next(filter_hp_blk *f, fract32 in, fract32 alpha);
fract32 filter_lp_blk_next(filter_lp_blk *f, fract32 in, fract32 alpha);

/* block-rate: divides only here */
void filter_bp_blk_prepare(filter_bp_blk *f, fract32 hp_freq, fract32 lp_freq);
fract32 filter_bp_blk_next(filter_bp_blk *f, fract32 in);

#endif
