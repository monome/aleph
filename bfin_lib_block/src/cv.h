#ifndef BFIN_LIB_BLOCK_CV_H
#define BFIN_LIB_BLOCK_CV_H

#include "types.h"

#define CV_CHANNELS 4

/* bring AD5686 out of reset. call once at boot before SPORT1/DMA enable. */
void init_cv(void);

/* DMA4 → SPORT1 setup / enable for 4-word CV bursts. */
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
 * pack shadow[0..3] into DMA words and start a 4-channel burst.
 * waits for any prior burst. returns 0 on kick, non-zero on wait timeout.
 */
u8 cv_commit(void);

/* 1 if DMA4 CV burst is still in flight. */
u8 cv_busy(void);

/* spin until !cv_busy(), with a short timeout. returns 0 ok, 1 on timeout. */
u8 cv_wait(void);

#endif
