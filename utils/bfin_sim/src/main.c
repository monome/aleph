// bfin toolchain
#include "ccblkfn.h"
#include "sysreg.h"
// aleph
#include "bfin_core.h"
#include "control.h"
#include "cv.h"
#include "gpio.h"
#include "init.h"
#include "module.h"

long long int dumcount = 0;

//-------------------------------
// main function
int main(void) {
  //u32 del;
  // turn on execution counter
  // default .crt does this for us
  //  __asm__ __volatile__("R0 = 0x32; SYSCFG = R0; CSYNC;":::"R0");

  // initialize clocks and power
  init_clocks();
  // configure programmable flags
  init_flags();  

  READY_LO;

  // intialize the sdram controller
  init_EBIU();
  // intialize the flash controller (which, weirdly, handles gpio)
  //  init_flash();  

  /// initialize the CV dac (reset) 
  init_cv();

  // intialize the sport0 for audio rx/tx
  init_sport0();
  // intialize the sport1 for cv out
  init_sport1();

  // intialize DMA for audio
  init_DMA();
  //  // put the spi back in slave mode to receive param changes from avr32
  init_spi_slave();
   
  // intialize the audio processing unit (assign memory)
  module_init();

  // assign interrupts
  init_interrupts();

  // begin audio transfers
  enable_DMA_sport0();  
  // begin cv transfers
  enable_DMA_sport1();  

  // initialize the codec
  init_1939();

  // leds on
  LED3_HI;
  LED4_HI;

  // signal the ready flag
  READY_HI;
  
  while(1) {
    // fixme: everything happens in ISRs!
    //    ;;

    /*
    //// TODO / FIXME: 
     update a param change FIFO here?
    for now, the answer is no:
    instead, we are asking avr32 to hold off sending params 
    for as long as the ready-pin is deasserted by frame or control change processing.
    */
    /// while frame processing 
    //    ctl_next_frame();	
    //    ctl_perform_last_change();			
  }
}
