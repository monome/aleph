#include "bfin_core.h"
#include "init.h"

//-------------------------------
// main function
int main(void) {
  u64 delay;
  // initialize clocks and power
  init_clocks();

  // configure programmable flags
  init_flags();  

  // intialize SPORT1
  init_sport1();

  // timers
  init_timers();

  // assign interrupts
  init_interrupts();

  // bring the DAC out of reset
  *pFIO_FLAG_D &= (0xffff ^ (1 << DAC_RESET_PIN));
  delay = 100000;
  while(delay > 0) { delay--; }
  *pFIO_FLAG_D |= (1 << DAC_RESET_PIN);
  

  while(1) {
    //;;
    //    while((*pSPORT1_STAT & 0x0040) == 0) {;;}  
    //    *pSPORT1_TX32 = 0xfedcba98;
    //  txUpdate();
    //    delay = 0;
    //    while(delay < 100000) { delay++; }
  }
}

