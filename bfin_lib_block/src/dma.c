#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "dma.h"

// use large descriptor mode to perform pingpong and deinterleave

typedef struct {
  void *next;        // NDPL, NDPH
  void *start;       // SAL, SAH
} dma_desc_t;

// for deinterleaving, inner loop over number of channels
#define X_COUNT AUDIO_CHANNELS
// each inner-loop increment, jump forward to the same frame in the next channel
#define X_MOD (AUDIO_SAMPLESIZE * AUDIO_FRAMES)
// outer loop over number of frames in a block
#define Y_COUNT AUDIO_FRAMES
// each outer-loop increment, jump back to the next frame in the first channel
#define Y_MOD (((1 - AUDIO_CHANNELS) * AUDIO_FRAMES + 1) * AUDIO_SAMPLESIZE)

// large descriptor list, first 4 registers only (NDPL, NDPH, SAL, SAH)
#define DMA_FLOW_DESC 0x7400
// NB: need interrupt on both TX and RX to ensure correct process order 
#define DMA_CONFIG ( DMA_FLOW_DESC | WDSIZE_32 | DI_EN | DMA2D )

dma_desc_t descRx1 = { NULL, inputChannels1 };
dma_desc_t descRx0 = { &descRx1, inputChannels0 };
dma_desc_t descTx1 = { NULL, outputChannels1 };
dma_desc_t descTx0 = { &descTx1, outputChannels0 };


void init_dma(void) {

  // map DMA1 to sport0 RX
  *pDMA1_PERIPHERAL_MAP = 0x1000;  
  // map DMA2 to sport0 TX
  *pDMA2_PERIPHERAL_MAP = 0x2000;
  
  // ping-pong
  descRx1.next = &descRx0;
  descTx1.next = &descTx0;

  *pDMA1_X_COUNT = X_COUNT;
  *pDMA1_X_MODIFY = X_MOD;
  *pDMA1_Y_COUNT = Y_COUNT;
  *pDMA1_Y_MODIFY = Y_MOD;
  *pDMA1_NEXT_DESC_PTR = &descRx0;
  *pDMA1_CONFIG = DMA_CONFIG | WNR;

  *pDMA2_X_COUNT = X_COUNT;
  *pDMA2_X_MODIFY = X_MOD;
  *pDMA2_Y_COUNT = Y_COUNT;
  *pDMA2_Y_MODIFY = Y_MOD;
  *pDMA2_NEXT_DESC_PTR = &descTx0;
  *pDMA2_CONFIG = DMA_CONFIG;
}
