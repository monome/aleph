#include <blackfin.h>
#include <cdefBF533.h>

#include "cv.h"
#include "gpio.h"

/* AD5686R 16-bit quad DAC — 24-bit write-and-update frames */
#define CV_DAC_COM_WRITE 0x3
#define CV_DAC_COM_LSHIFT 20
#define CV_DAC_ADDR_LSHIFT 16

/* continuous autobuffer (FLOW_1 = 0x1000), like frame lib but 4 words;
 * WDSIZE_32 memory words, SPORT1 SLEN sends 24 bits */
#define CV_DMA_FLOW_1 0x1000
#define CV_DMA_CONFIG (WDSIZE_32 | CV_DMA_FLOW_1)

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
  *pDMA4_X_COUNT = CV_CHANNELS;
  *pDMA4_X_MODIFY = 4;
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
