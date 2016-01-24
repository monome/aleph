#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "dma.h"


typedef struct {
   void *next;
   void *start;
   short config;
   short x_count;
   short x_mod;
   short y_count;
   short y_mod;
} dma_desc_t;

/* #define CHANNELS 4 */
/* #define BLOCKSIZE 16 */
/* #define SAMPLESIZE 4 // sizeof(fract32) */

#define X_COUNT CHANNELS
#define X_MOD (SAMPLESIZE * BLOCKSIZE)
#define Y_COUNT BLOCKSIZE
#define Y_MOD (((1 - CHANNELS) * BLOCKSIZE + 1) * SAMPLESIZE)

#define DMA_FLOW_DESC 0x7700
#define DMA_CONFIG (WDSIZE_32 | DMA_FLOW_DESC | DMAEN | DI_EN | DMA2D )

fract32 inputChannels0[CHANNELS][BLOCKSIZE];
fract32 inputChannels1[CHANNELS][BLOCKSIZE];

fract32 outputChannels0[CHANNELS][BLOCKSIZE];
fract32 outputChannels1[CHANNELS][BLOCKSIZE];

dma_desc_t descRx1 = { NULL, inputChannels1, WNR | DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };
dma_desc_t descRx0 = { &descRx1, inputChannels0, WNR | DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };

dma_desc_t descTx1 = { NULL, outputChannels1, DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };
dma_desc_t descTx0 = { &descTx1, outputChannels0, DMA_CONFIG, X_COUNT, X_MOD, Y_COUNT, Y_MOD };



void init_dma(void) {

  // Map DMA1 to Sport0 RX
  *pDMA1_PERIPHERAL_MAP = 0x1000;  
  // Map DMA2 to Sport0 TX
  *pDMA2_PERIPHERAL_MAP = 0x2000;
  
  // ping-pong
  descRx1.next = &descRx0;
  descTx1.next = &descTx0;

  *pDMA1_NEXT_DESC_PTR = &descRx0;
  *pDMA2_NEXT_DESC_PTR = &descTx0;
}



// DMA flow modes
/* #define FLOW_AUTO 0x1000 */
/* #define FLOW_DESC 0x7700 */

/* void init_dma(void) {  */

  
/*   // Map DMA1 to Sport0 RX */
/*   *pDMA1_PERIPHERAL_MAP = 0x1000;   */
/*   // Map DMA2 to Sport0 TX */
/*   *pDMA2_PERIPHERAL_MAP = 0x2000; */


/*   // NB: need interrupt on both TX and RX to ensure correct process order */
  
/*   // dma1: rx */
/*   *pDMA1_CONFIG = WNR | WDSIZE_32 | DI_EN | FLOW_AUTO; */
/*   *pDMA1_START_ADDR = (void *)audioRxBuf; */
/*   *pDMA1_X_COUNT = 4 * BLOCKSIZE; */
/*   *pDMA1_X_MODIFY = 4; */
  
/*   // dma 2: tx */
/*   *pDMA2_CONFIG = WDSIZE_32 | DI_EN | FLOW_AUTO; */
/*   *pDMA2_START_ADDR = (void *)audioTxBuf; */
/*   *pDMA2_X_COUNT = 4 * BLOCKSIZE; */
/*   *pDMA2_X_MODIFY = 4; */

/* } */
