// bfin toolchain
#include "ccblkfn.h"
#include "sysreg.h"
// aleph
#include "bfin_core.h"
#include "init.h"
#include "module.h"

long long int dumcount = 0;

//-------------------------------
// main function
int main(void) {
  // turn on execution counter
  // default .crt does this for us
  //  __asm__ __volatile__("R0 = 0x32; SYSCFG = R0; CSYNC;":::"R0");

  // initialize clocks and power
  init_clocks();
  // configure programmable flags
  init_flags();  
  // intialize the sdram controller
  init_EBIU();
  // intialize the flash controller (which, weirdly, handles gpio)
  init_flash();
  // initialize the codec (spi in master, blast config regs, disable spi)


  ////////////////////
  /////////////// TEST : do this a lot
  
  /*
  *pFlashA_PortB_Data = 0x3f;
  while(dumcount < 1000000) {
    init_1836();
    dumcount++;
  }
  *pFlashA_PortB_Data = 0;
  */

  /////////////////////
  ////////////////////////
  //init_1836();

  // intialize the sport0 for audio rx/tx
  init_sport0();
  // intialize the DMA to shove audio data around
  init_DMA();
  // put the spi back in slave mode to receive param changes from avr32
  init_spi_slave();
  // intialize the audio processing unit (assign memory)
  module_init();
  // assign interrupts
  init_interrupts();
  // begin audio transfers
  enable_DMA_sport0();  

  while(1) {
    // fixme: everything happens in ISRs!
    ;;
  }
}

