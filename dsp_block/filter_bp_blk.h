/* filter_bp_blk.h
 * aleph dsp_block
 *
 * Block-rate bandpass (1-pole HP then 1-pole LP). Classic sibling types
 * live in dsp/ricks_tricks.h (hpf / lpf / bpf).
 *
 * Coefficients are supplied by the caller (see filter_bp_alpha_tab.h) and
 * stored by set_alpha(); next() applies them only, so the sample loop has
 * no divides.
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

void filter_hp_blk_init(filter_hp_blk *f);
void filter_lp_blk_init(filter_lp_blk *f);
void filter_bp_blk_init(filter_bp_blk *f);

/* alpha is an explicit parameter — no divide inside */
fract32 filter_hp_blk_next(filter_hp_blk *f, fract32 in, fract32 alpha);
fract32 filter_lp_blk_next(filter_lp_blk *f, fract32 in, fract32 alpha);

/* block-rate: store the coefficients used by the following sample loop */
void filter_bp_blk_set_alpha(filter_bp_blk *f, fract32 hpAlpha,
			     fract32 lpAlpha);
fract32 filter_bp_blk_next(filter_bp_blk *f, fract32 in);

#endif
