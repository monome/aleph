// bfin toolchain
#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "audio.h"
#include "module.h"
#include "dma.h"
#include "gpio.h"
#include "isr.h"
#include "serial.h"
#include "types.h"


void init_clock(void) { 
  // VCO = MSEL * CLKIN
  // ccore clock is 108MHz
  // so MSEL = 19 for 512Mhz processor clock
  *pPLL_CTL = 0x2600;
  ssync();
}

void init_ebiu(void) { 
  // initalize EBIU control registers to enable all banks
  *pEBIU_AMBCTL1 = 0xFFFFFF02;
  ssync();
  *pEBIU_AMGCTL = 0x00FF;
  ssync();
  
  // Check if already enabled
  if( SDRS != ((*pEBIU_SDSTAT) & SDRS) ) {
    return;
  }
  
  //SDRAM Refresh Rate Control Register
  // for 108Mhz system clock:
  *pEBIU_SDRRC = 835;
  
  //SDRAM Memory Bank Control Register
  *pEBIU_SDBCTL = 0x0025; //1.7	64 MB bank size
  
  //SDRAM Memory Global Control Register
  *pEBIU_SDGCTL = 0x0091998d;
  ssync();
}




// initialize programmable flags
void init_flags(void) {
  // outputs
  *pFIO_DIR = 0;
  *pFIO_DIR |= CODEC_RESET_UNMASK;
  *pFIO_DIR |= CODEC_RESET_UNMASK;
  *pFIO_DIR |= CV_DAC_RESET_UNMASK;
  *pFIO_DIR |= CV_DAC_LDAC_UNMASK;
  *pFIO_DIR |= READY_UNMASK;
  *pFIO_DIR |= LED3_UNMASK;
  *pFIO_DIR |= LED4_UNMASK;
}

// configure audio codec ( AD1939 )
void init_codec(void) { 
  volatile u32 del;

  //// reset codec
  *pFIO_FLAG_D &= CODEC_RESET_MASK;
  del = 100; while(del--) { ;; } 
  *pFIO_FLAG_D |= (0xffff ^ CODEC_RESET_MASK);
  del = 10000; while(del--) { ;; } 
  return;

} // init_codec


void enable_dma_sport0(void) { 
  *pDMA2_CONFIG	= (*pDMA2_CONFIG | DMAEN);
  *pDMA1_CONFIG	= (*pDMA1_CONFIG | DMAEN);
  *pSPORT0_TCR1 	= (*pSPORT0_TCR1 | TSPEN);
  *pSPORT0_RCR1 	= (*pSPORT0_RCR1 | RSPEN);
}


int main(void) { 

  init_clock();
  init_flags();
  READY_LO;
  
  init_ebiu();

  init_spi_slave();
    
  init_sport0();
  init_interrupts();
  init_dma();


  module_init();
  
  enable_dma_sport0();

  // reset the codec
  init_codec();


  LED3_HI;
  LED4_HI;

  READY_HI;
  
  while(1) { 

    if(audioTxDone && audioRxDone && processAudio) {

      READY_LO;
      // actually this doesn't work in User mode.
      // could move block processing to an interrupt if we really want to do this.
      // but if possible it would be better to allow SPI interrupts to happen

      // disable interrupts:
      // int i=0; 
      // asm volatile ("cli %0; csync;" : "+d"(i));
	
      module_process_block(audioIn, audioOut);
      audioTxDone = 0;
      audioRxDone = 0;

      // reenable interrupts
      //      asm volatile ("sti %0; csync;" : "+d"(i));
      READY_HI;
    }
  }
  
  return 0;
}
