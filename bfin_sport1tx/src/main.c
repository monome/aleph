#include "bfin_core.h"
#include "init.h"

//----- external data (initialized here)
//txbuf_t sport1TxBuf;
volatile u8 sport1TxBuf[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

// enable sport1 dma for writing to dacs
void enable_sport1_dma() {
  // enable DMA
  *pDMA4_CONFIG	= (*pDMA4_CONFIG | DMAEN);
  // enable sport1 tx
  *pSPORT1_TCR1 = (*pSPORT1_TCR1 | TSPEN);
}


//-------------------------------
// main function
int main(void) {
  // turn on execution counter
  // default .crt does this for us
  //  __asm__ __volatile__("R0 = 0x32; SYSCFG = R0; CSYNC;":::"R0");

  /// intialize transmit buffer data;

  // intialize the command words in the tx data
  //  sport1TxBuf.dacs[0].com = 0x20;
  //  sport1TxBuf.dacs[1].com = 0x20;
  //  sport1TxBuf.dacs[2].com = 0x20;
  
  // initialize clocks and power
  init_clocks();
  // configure programmable flags
  //init_flags();  
  init_sport1();
  // assign interrupts
  init_interrupts();

  // enable sport1 and associated DMA
  enable_sport1_dma();

  while(1) {
    ;;
  }
}

