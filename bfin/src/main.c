// bfin toolchain
#include "ccblkfn.h"
#include "sysreg.h"
// aleph
#include "bfin_core.h"
#include "init.h"
#include "leds.h"
#include "module.h"

long long int dumcount = 0;

//// testing
static fract32 add_fr(fract32 x, fract32 y) {
  fract32 res;
  res = add_fr1x32(x, y);
  return res;
}
////

//-------------------------------
// main function
int main(void) {
  u32 del;
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
  //  init_flash();
  // initialize the codec (spi in master, blast config regs, disable spi)

  init_1939();

  // intialize the sport0 for audio rx/tx
  init_sport0();
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


  //// test: leds on
  SET_LED3;
  SET_LED4;
  
  while(1) {
    // fixme: everything happens in ISRs!
    ;;
  }
}
