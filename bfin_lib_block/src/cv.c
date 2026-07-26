#include <blackfin.h>
#include <cdefBF533.h>

#include "cv.h"
#include "gpio.h"

/* AD5686R 16-bit quad DAC — same wire format as bfin_lib */
#define CV_DAC_COM_WRITE 0x3
#define CV_DAC_COM_LSHIFT 20
#define CV_DAC_ADDR_LSHIFT 16

/* DMA4_IRQ_STATUS: bit0 = DMA_DONE, bit3 = DMA_RUN */
#define CV_DMA_DONE 0x0001
#define CV_DMA_RUN 0x0008

/* stop-mode 32-bit transfers (FLOW = 0) */
#define CV_DMA_CONFIG WDSIZE_32

#define CV_WAIT_SPINS 100000

static fract32 cvShadow[CV_CHANNELS];
static volatile u32 cvTxWords[CV_CHANNELS];

static u32 cv_pack(u8 ch, fract32 val) {
  u32 buf;

  buf = 0;
  buf |= (CV_DAC_COM_WRITE << CV_DAC_COM_LSHIFT);
  buf |= ((1 << ch) << CV_DAC_ADDR_LSHIFT);
  buf |= (val >> 15) & 0xffff;
  /* extra bit for FS timing kludge (need 25 clocks) */
  return buf << 1;
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

  /* map DMA4 to SPORT1 TX */
  *pDMA4_PERIPHERAL_MAP = 0x4000;
  *pDMA4_CONFIG = CV_DMA_CONFIG;
  *pDMA4_START_ADDR = (void *)(&cvTxWords[0]);
  *pDMA4_X_COUNT = CV_CHANNELS;
  *pDMA4_X_MODIFY = 4;
}

void enable_dma_sport1(void) {
  /* SPORT1 TX; DMA4 is kicked per cv_commit() in stop mode */
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
  return (*pDMA4_IRQ_STATUS & CV_DMA_RUN) != 0;
}

u8 cv_wait(void) {
  u32 spins = CV_WAIT_SPINS;

  while(cv_busy()) {
    if(spins == 0) {
      return 1;
    }
    spins--;
  }
  return 0;
}

u8 cv_commit(void) {
  u8 ch;
  u8 timed_out;

  timed_out = cv_wait();

  for(ch = 0; ch < CV_CHANNELS; ++ch) {
    cvTxWords[ch] = cv_pack(ch, cvShadow[ch]);
  }

  /* clear completion sticky; reprogram and start one-shot burst */
  *pDMA4_IRQ_STATUS = CV_DMA_DONE;
  *pDMA4_START_ADDR = (void *)(&cvTxWords[0]);
  *pDMA4_X_COUNT = CV_CHANNELS;
  *pDMA4_X_MODIFY = 4;
  *pDMA4_CONFIG = CV_DMA_CONFIG | DMAEN;

  return timed_out;
}
