#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "dma.h"
#include "gpio.h"
#include "isr.h"

// TEST: flip leds
long long unsigned int ledCounterRx = 0;
long long unsigned int ledCounterTx = 0;

// flags for pingpong processing
static volatile u8 inBufFlag = 0;
static volatile u8 outBufFlag = 0;

__attribute((interrupt_handler)) 
void sport0_rx_isr(void) {
  // u16 i;

  // copy input for new block
  /* for(i=0; i<BLOCKSIZE*4; i++) {  */
  /*   audioProcessInBuf[i] = audioRxBuf[i] << 8; */
  /* } */

  if(inBufFlag) {
    audioIn = inputChannels1;
    inBufFlag = 0;
  } else {
    audioIn = inputChannels0;
    inBufFlag = 1;
  }
  
  if (++ledCounterRx > 4000) {
    ledCounterRx = 0;
    LED3_TOGGLE;
  }

  audioRxDone = 1;
  
  *pDMA1_IRQ_STATUS = 0x0001;
  ssync();
}


__attribute((interrupt_handler)) 
void sport0_tx_isr(void) {
  // u16 i;
  
  if(outBufFlag) {
    audioOut = outputChannels1;
    outBufFlag = 0;
  } else {
    audioOut = outputChannels0;
    outBufFlag = 0;
  }

  if (++ledCounterTx > 6000) {
    ledCounterTx = 0;
    LED4_TOGGLE;
  }
  
  // copy output from last block
  /* for(i=0; i<BLOCKSIZE*4; i++) { */
  /*   audioTxBuf[i] = audioProcessOutBuf[i] >> 8; */
  /* } */

  audioTxDone = 1;
  
  *pDMA2_IRQ_STATUS = 0x0001;
  ssync();
}
  
// assign interrupts
void init_interrupts(void) {
  int i=0;

  // sport0 rx (dma1) -> ID3 = IVG10
  // sport1 tx (dma2) -> ID2 = IVG9
  *pSIC_IAR1 = 0x33322231;

  // assign handlers to vectors
  *pEVT10 = sport0_rx_isr;
  *pEVT9 = sport0_tx_isr;

  // unmask DMA1 and DMA2
  *pSIC_IMASK=0x00000600;
  
  // unmask vectors in the core event processor
  asm volatile ("cli %0; bitset(%0, 9); bitset(%0, 10); sti %0; csync;": "+d"(i));

}
