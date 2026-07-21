/* filter_bp_blk.c
 * aleph dsp_block
 */

#include "filter_bp_blk.h"

void filter_hp_blk_init(filter_hp_blk *f) {
  f->lastIn = 0;
  f->lastOut = 0;
}

void filter_lp_blk_init(filter_lp_blk *f) {
  f->lastOut = 0;
}

void filter_bp_blk_init(filter_bp_blk *f) {
  filter_hp_blk_init(&(f->hp));
  filter_lp_blk_init(&(f->lp));
  f->hp_alpha = 0;
  f->lp_alpha = 0;
}

fract32 filter_hp_blk_next(filter_hp_blk *f, fract32 in, fract32 alpha) {
  fract32 out = add_fr1x32(mult_fr1x32x32(alpha, f->lastOut),
			   mult_fr1x32x32(alpha, sub_fr1x32(in, f->lastIn)));
  f->lastOut = out;
  f->lastIn = in;
  return out;
}

fract32 filter_lp_blk_next(filter_lp_blk *f, fract32 in, fract32 alpha) {
  fract32 out =
      add_fr1x32(mult_fr1x32x32(alpha, in),
		 mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), f->lastOut));
  f->lastOut = out;
  return out;
}

void filter_bp_blk_prepare(filter_bp_blk *f, fract32 hp_freq, fract32 lp_freq) {
  f->hp_alpha = filter_bp_blk_hpf_freq_calc(hp_freq);
  f->lp_alpha = filter_bp_blk_lpf_freq_calc(lp_freq);
}

fract32 filter_bp_blk_next(filter_bp_blk *f, fract32 in) {
  fract32 hp_out = filter_hp_blk_next(&(f->hp), in, f->hp_alpha);
  return filter_lp_blk_next(&(f->lp), hp_out, f->lp_alpha);
}
