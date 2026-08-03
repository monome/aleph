#include <blackfin.h>
#include <cdefBF533.h>

#include "cv.h"
#include "gpio.h"

/* AD5686R 16-bit quad DAC — 24-bit write-and-update frames */
#define CV_DAC_COM_WRITE 0x3
#define CV_DAC_COM_LSHIFT 20
#define CV_DAC_ADDR_LSHIFT 16

/*
 * SPORT1 drives the DAC from the secondary data pin (DT1SEC), so TXSE must
 * stay set; there is no way to disable the primary transmitter, which TSPEN
 * enables. primary and secondary shift out together in one frame, and DMA4
 * feeds both: it fetches two words per frame, primary first, then secondary.
 * DT1PRI goes nowhere on this board, so its word is discarded.
 *
 * 2D autobuffer sends each channel word down both halves of the frame, so the
 * DAC gets all four channels from a 4-word buffer:
 *
 *   inner loop  X_COUNT = 2, X_MODIFY = 0  -> same word twice (pri + sec)
 *   outer loop  Y_COUNT = 4, Y_MODIFY = 4  -> advance one word per frame
 *
 * blackfin applies Y_MODIFY instead of X_MODIFY on the last inner iteration,
 * giving the fetch order c0 c0 c1 c1 c2 c2 c3 c3, repeating forever. reading
 * each word twice also makes this immune to which half of the frame the DMA
 * happens to start on.
 *
 * WDSIZE_32 memory words; SPORT1 SLEN sends 24 bits of each.
 */
#define CV_DMA_FLOW_1 0x1000
#define CV_DMA_CONFIG (WDSIZE_32 | DMA2D | CV_DMA_FLOW_1)

static fract32 cvShadow[CV_CHANNELS];
static volatile u32 cvTxWords[CV_CHANNELS];

static u32 cv_pack(u8 ch, fract32 val) {
  u32 buf;

  buf = 0;
  buf |= (CV_DAC_COM_WRITE << CV_DAC_COM_LSHIFT);
  buf |= ((1 << ch) << CV_DAC_ADDR_LSHIFT);
  buf |= (val >> 15) & 0xffff;
  return buf;
}

void init_cv(void) {
  u32 delay;

  CV_DAC_RESET_LO;
  delay = 100000;
  while(delay > 0) {
    delay--;
  }
  CV_DAC_RESET_HI;
}

void init_dma_cv(void) {
  u8 ch;

  for(ch = 0; ch < CV_CHANNELS; ++ch) {
    cvShadow[ch] = 0;
    cvTxWords[ch] = cv_pack(ch, 0);
  }

  /* map DMA4 to SPORT1 TX; loop all four channel words forever */
  *pDMA4_PERIPHERAL_MAP = 0x4000;
  *pDMA4_CONFIG = CV_DMA_CONFIG;
  *pDMA4_START_ADDR = (void *)(&cvTxWords[0]);
  *pDMA4_X_COUNT = 2;
  *pDMA4_X_MODIFY = 0;
  *pDMA4_Y_COUNT = CV_CHANNELS;
  *pDMA4_Y_MODIFY = 4;
}

void enable_dma_sport1(void) {
  *pDMA4_CONFIG = (*pDMA4_CONFIG | DMAEN);
  *pSPORT1_TCR1 = (*pSPORT1_TCR1 | TSPEN);
}

void cv_set(u8 ch, fract32 val) {
  if(ch >= CV_CHANNELS) {
    return;
  }
  cvShadow[ch] = val;
}

fract32 cv_get(u8 ch) {
  if(ch >= CV_CHANNELS) {
    return 0;
  }
  return cvShadow[ch];
}

u8 cv_busy(void) {
  /* DMA runs continuously; commits only refresh the TX buffer. */
  return 0;
}

u8 cv_wait(void) {
  return 0;
}

u8 cv_commit(void) {
  u8 ch;

  for(ch = 0; ch < CV_CHANNELS; ++ch) {
    cvTxWords[ch] = cv_pack(ch, cvShadow[ch]);
  }
  return 0;
}
