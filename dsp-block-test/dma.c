#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "dma.h"

// DMA flow modes
#define FLOW_AUTO 0x1000
#define FLOW_DESC 0x7700

// #define DMA_CONFIG_RX (WDSIZE_32 | WNR | DMAEN | FLOW_DESC)
// #define DMA_CONFIG_TX (WDSIZE_32 | DMAEN | FLOW_DESC)

void init_dma(void) { 

  
  // Map DMA1 to Sport0 RX
  *pDMA1_PERIPHERAL_MAP = 0x1000;  
  // Map DMA2 to Sport0 TX
  *pDMA2_PERIPHERAL_MAP = 0x2000;


  // dma1: rx
  *pDMA1_CONFIG = WNR | WDSIZE_32 | DI_EN | FLOW_AUTO;
  /// hm... try interrupt on TX isntead?
  // *pDMA1_CONFIG = WNR | WDSIZE_32 | FLOW_AUTO;
  *pDMA1_START_ADDR = (void *)audioRxBuf;
  *pDMA1_X_COUNT = 4 * BLOCKSIZE;
  *pDMA1_X_MODIFY = 4;
  
  // dma 2: tx
  ///*pDMA2_CONFIG = WDSIZE_32 | FLOW_AUTO;
  /// hm... try interrupt on TX instead/also?
  *pDMA2_CONFIG = WDSIZE_32 | DI_EN | FLOW_AUTO;
  *pDMA2_START_ADDR = (void *)audioTxBuf;
  *pDMA2_X_COUNT = 4 * BLOCKSIZE;
  *pDMA2_X_MODIFY = 4;

}
