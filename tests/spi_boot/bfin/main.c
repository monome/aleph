#include "BF533 Flags.h"
#include "ccblkfn.h"
#include "sysreg.h"

// global vairables
extern unsigned char gButState = 0;
extern unsigned int gLedState = 0;

int main(void)
{
  unsigned int ledState = 0;
  unsigned char butState = 0;

  //	sysreg_write(reg_SYSCFG, 0x32);		//Initialize System Configuration Register
  Init_Flags();
  Init_Timers();
  Init_EBIU();
  Init_Flash();
  Init_Interrupts();
  
  while(1) {
    if ((ledState != gLedState) || (butState != gButState)) {
      /*
      ledState = gLedState;
      
      butState = gButState;
      
      
      if(gButState > 0) {
		*pFlashA_PortB_Data = ~ledState & 0x3f;
      } else {
		*pFlashA_PortB_Data = ledState;
      }
      
      */
      //      *pFlashA_PortB_Data = gButState;
      *(pFIO_FLAG_D) &= 0x3fff;
      *(pFIO_FLAG_D) |= (gLedState << 14) ;
    }
  }
}

