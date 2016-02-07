// bfin toolchain
#include <blackfin.h>
#include <cdefBF533.h>
#include "ccblkfn.h"

#include "audio.h"
#include "clock_ebiu.h"
#include "cycle_count_aleph.h"
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
//static volatile u64 startCycleCount;


int main(void) { 
  // start execution counter
  //  __asm__ __volatile__("R0 = 0x32; SYSCFG = R0; CSYNC;":::"R0");
  
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


      /// hm...
      // LO/HI block probably has the result of stalling param changes anyway..
      // we're getting worse occurences of avr32 event queue stalls,
      // so try not stalling it at all...
      //READY_LO;
      
      /**
	 actually, we can't manipulate CLI/STI in User mode.
	 if we want to, must move block processing to an interrupt itself.
	 but if it is feasible, would be good to continue collecting SPI changes
	 during the block process routine.
      */

     
      // disable interrupts:
      // int i=0; 
      // asm volatile ("cli %0; csync;" : "+d"(i));


      // asm volatile ("%0 = CYCLES; %H0 = CYCLES2;" : "=d" (startCycleCount));
      /**
	 attempting to use the execution cycle counter here. 
	 but enabling this is causing weird problems - 
	 we start missing most param changes...
       */

      module_process_block(audioIn, audioOut);
      audioTxDone = 0;
      audioRxDone = 0;
      
      // asm volatile ("%0 = CYCLES; %H0 = CYCLES2;" : "=d" (audioCycleCount));
      // audioCycleCount -= startCycleCount;

      // reenable interrupts
      //      asm volatile ("sti %0; csync;" : "+d"(i));

      // hm...
      //READY_HI;
    }
  }
  
  return 0;
}
