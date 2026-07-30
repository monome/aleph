#ifndef BFIN_LIB_BLOCK_CV_H
#define BFIN_LIB_BLOCK_CV_H

#include "types.h"

#define CV_CHANNELS 4

/* bring AD5686 out of reset. call once at boot before SPORT1/DMA enable. */
void init_cv(void);

/* DMA4 → SPORT1 continuous 4-word autobuffer setup / enable. */
void init_dma_cv(void);
void enable_dma_sport1(void);

/*
 * store a channel value in the software shadow.
 * ch in [0, 3]; val in [0, 0x7fffffff].
 * does not touch the DAC until cv_commit().
 */
void cv_set(u8 ch, fract32 val);

/* read back shadow. */
fract32 cv_get(u8 ch);

/*
 * pack shadow[0..3] into the DMA TX words.
 * DMA4 autobuffers those words continuously; no DMA restart.
 * always returns 0.
 */
u8 cv_commit(void);

/* always 0 — DMA runs continuously; kept for API compatibility. */
u8 cv_busy(void);

/* always 0 — no burst wait; kept for API compatibility. */
u8 cv_wait(void);

#endif
