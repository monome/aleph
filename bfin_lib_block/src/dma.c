#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "dma.h"

#if DMA_DEINTERLEAVE_PINGPONG
// use descriptor mode to perform pingpong and deinterleave

typedef struct {
   void *next;
   void *start;
   short config;
   short x_count;
   short x_mod;
   short y_count;
   short y_mod;
} dma_desc_t;

// for deinterleaving, inner loop over number of channels
#define X_COUNT AUDIO_CHANNELS
// each inner-loop increment, jump forward to the same frame in the next channel
#define X_MOD (AUDIO_SAMPLESIZE * AUDIO_FRAMES)
// outer loop over number of frames in a block
#define Y_COUNT AUDIO_FRAMES
// each outer-loop increment, jump back to the next frame in the first channel
#define Y_MOD (((1 - AUDIO_CHANNELS) * AUDIO_FRAMES + 1) * AUDIO_SAMPLESIZE)

#define DMA_FLOW_DESC 0x7700
// NB: need interrupt on both TX and RX to ensure correct process order 
#define DMA_CONFIG ( WDSIZE_32 | DMA_FLOW_DESC | DI_EN | DMA2D )

dma_desc_t descRx1 = { NULL, inputChannels1, WNR | DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };
dma_desc_t descRx0 = { &descRx1, inputChannels0, WNR | DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };

dma_desc_t descTx1 = { NULL, outputChannels1, DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };
dma_desc_t descTx0 = { &descTx1, outputChannels0, DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };


void init_dma(void) {

  // map DMA1 to sport0 RX
  *pDMA1_PERIPHERAL_MAP = 0x1000;  
  // map DMA2 to sport0 TX
  *pDMA2_PERIPHERAL_MAP = 0x2000;
  
  // ping-pong
  descRx1.next = &descRx0;
  descTx1.next = &descTx0;

  *pDMA1_NEXT_DESC_PTR = &descRx0;
  *pDMA2_NEXT_DESC_PTR = &descTx0;
}

#else // 1d, autobuffer mode (no de-interleave, no pingpong

// DMA flow modes
 #define FLOW_AUTO 0x1000 

 void init_dma(void) {  
  
   // map DMA1 to Sport0 RX 
   *pDMA1_PERIPHERAL_MAP = 0x1000;   
   // map DMA2 to Sport0 TX 
   *pDMA2_PERIPHERAL_MAP = 0x2000; 

   // NB: need interrupt on both TX and RX to ensure correct process order 
   // dma1: rx 
   *pDMA1_CONFIG = WNR | WDSIZE_32 | DI_EN | FLOW_AUTO; 
   *pDMA1_START_ADDR = (void *)audioRxBuf; 
   *pDMA1_X_COUNT = AUDIO_CHANNELS * AUDIO_FRAMES; 
   *pDMA1_X_MODIFY = AUDIO_SAMPLESIZE; 
  
   // dma 2: tx 
   *pDMA2_CONFIG = WDSIZE_32 | DI_EN | FLOW_AUTO; 
   *pDMA2_START_ADDR = (void *)audioTxBuf; 
   *pDMA2_X_COUNT = AUDIO_CHANNELS * AUDIO_FRAMES; 
   *pDMA2_X_MODIFY = AUDIO_SAMPLESIZE; 

 } 

#endif
