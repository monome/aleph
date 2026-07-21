/* filter_1p_blk.h
 * aleph dsp_block
 *
 * Block-rate one-pole lowpass (param slew). Classic sample-rate sibling:
 * dsp/filter_1p.h (filter_1p_lo).
 */

#ifndef _ALEPH_DSP_BLOCK_FILTER_1P_BLK_H_
#define _ALEPH_DSP_BLOCK_FILTER_1P_BLK_H_

#include "fix.h"
#include "types.h"

typedef struct _filter_1p_lo_blk {
  fract32 c; /* block-rate pole: y = x + c*(y - x) */
  fract32 x; /* target */
  fract32 y; /* filtered / prepared value */
  u16 blocksize;
} filter_1p_lo_blk;

void filter_1p_lo_blk_init(filter_1p_lo_blk *f, fract32 in, u16 blocksize);
/* sample_slew is the classic per-sample integrator coeff from bees;
 * stored c becomes sample_slew^blocksize. */
void filter_1p_lo_blk_set_slew(filter_1p_lo_blk *f, fract32 sample_slew);
void filter_1p_lo_blk_in(filter_1p_lo_blk *f, fract32 val);
/* one block-rate slew step; updates y */
void filter_1p_lo_blk_prepare(filter_1p_lo_blk *f);
/* return prepared y (no further slew) */
fract32 filter_1p_lo_blk_next(filter_1p_lo_blk *f);
u8 filter_1p_lo_blk_sync(filter_1p_lo_blk *f);

#endif
