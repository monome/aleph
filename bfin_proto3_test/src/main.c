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

//static volatile u8* sdram = (u8*)SDRAM_ADDRESS;

//-------------------------------
// main function
int main(void) {
  u64 dum = 0;
  //  u64 i = 0;
  //  u64 errors = 0;
  //  u64 testNum;
  //  u64 tmp;

  //  u32* pfiod = (u32*)pFIO_FLAG_D;

  // turn on execution counter
  // default .crt does this for us
  //  __asm__ __volatile__("R0 = 0x32; SYSCFG = R0; CSYNC;":::"R0");

  /// intialize transmit buffer data;

  // intialize the command words in the tx data
  //  sport1TxBuf.dacs[0].com = 0x20;
  //  sport1TxBuf.dacs[1].com = 0x20;
  //  sport1TxBuf.dacs[2].com = 0x20;
  
  // initialize clocks and powerproto3_test/src/i
  init_clocks();
  // intialize external memory bus
  //  init_EBIU();
  // configure programmable flags
  init_flags();  
 
  // sport1 (tx to DACs)
  //  init_sport1();
  // assign interrupts
  init_interrupts();


  /// leds are pf 14 and 15
  *pFIO_FLAG_D &= 0xbfff;

  // init spi in slave mode
  init_spi_slave();

  //  *pFIO_FLAG_D = 0x8000;

  // enable sport1 and associated DMA
  //  enable_sport1_dma();



 // test sdram
  /*  
  testNum = SDRAM_BYTES;

  *pFIO_FLAG_D = 0x8000;

  for (i=0; i < testNum; i++ ) {
    *(sdram + i) = (u8)(i & 0xff);
    //    *pFIO_FLAG_D = (b & 0x3) << 14;
  }

  for (i=0; i < testNum; i++ ) {
    tmp = (u64)(*(sdram + i));
    //    *pFIO_FLAG_D = (i & 0x3) << 14;
    if(tmp != (i & 0xff)) {
      errors++;
    }
  }

  if (errors > 0) {
    *pFIO_FLAG_D = 0x3fff;    
  } else {
    *pFIO_FLAG_D = 0xc000;
  }
  */


  //    *pFIO_FLAG_D = 0x3fff;
  //  *pFIO_FLAG_D = 0xc000;


  while(1) {
    dum++;
    if(dum == 0x100000) {
        *pFIO_FLAG_D &= 0xbfff;
    }
    if(dum == 0x200000) {
        *pFIO_FLAG_D |= 0x4000;
	dum = 0;
    }
  }


}

