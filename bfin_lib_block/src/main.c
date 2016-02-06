// bfin toolchain
#include <blackfin.h>
#include <cdefBF533.h>
//#include <cycle_count_bf.h>
#include "ccblkfn.h"

#include "audio.h"
#include "clock_ebiu.h"
#include "dma.h"
#include "gpio.h"
#include "isr.h"
#include "module.h"
#include "serial.h"
#include "types.h"

#ifndef MODULE_BLOCKSIZE
#error "MODULE_BLOCKSIZE not defined!"
#endif

// temp cycle count
static volatile u32 startCycleCount;


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
      // but maybe it is better to allow SPI interrupts to happen...

      // disable interrupts:
      // int i=0; 
      // asm volatile ("cli %0; csync;" : "+d"(i));

      _START_CYCLE_COUNT(startCycleCount);
      
      module_process_block(audioIn, audioOut);
      audioTxDone = 0;
      audioRxDone = 0;

      _STOP_CYCLE_COUNT(audioCycleCount, startCycleCount);

      // reenable interrupts
      //      asm volatile ("sti %0; csync;" : "+d"(i));
      READY_HI;
    }
  }
  
  return 0;
}
