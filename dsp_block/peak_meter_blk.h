/* peak_meter_blk.h
 * aleph dsp_block
 *
 * Absolute peak-hold with block-rate decay toward silence (VU-style bar).
 */

#ifndef _ALEPH_DSP_BLOCK_PEAK_METER_BLK_H_
#define _ALEPH_DSP_BLOCK_PEAK_METER_BLK_H_

#include "fix.h"
#include "types.h"

/* ~200 ms release at 48 kHz (sample-rate pole; raised to N in set_decay). */
#define PEAK_METER_BLK_DECAY_DEFAULT ((fract32)0x7FFC95FF)

typedef struct _peak_meter_blk {
  fract32 peak;  /* current hold level in [0, FR32_MAX] */
  fract32 decay; /* block-rate 1-pole coeff toward 0 */
  u16 blocksize;
} peak_meter_blk;

void peak_meter_blk_init(peak_meter_blk *m, u16 blocksize);
/* sample_decay is a classic per-sample integrator pole; stored decay
 * becomes sample_decay^blocksize (same idea as filter_1p_lo_blk_set_slew). */
void peak_meter_blk_set_decay(peak_meter_blk *m, fract32 sample_decay);
/* peak = decay * peak (toward silence); once per block */
void peak_meter_blk_prepare(peak_meter_blk *m);
/* a = abs(in); if (a > peak) peak = a */
void peak_meter_blk_next(peak_meter_blk *m, fract32 in);
fract32 peak_meter_blk_read(peak_meter_blk *m);

#endif
