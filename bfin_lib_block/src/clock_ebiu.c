/*
  clock_ebiu.c

  definitions for clock and SDRAM setup.
  
  don't edit these unless you have a really good reason!

  for example, porting to different hardware...

 */

#include <cdefBF533.h>
#include "clock_ebiu.h"

void init_clock(void) { 
  // VCO = MSEL * CLKIN
  // ccore clock is 108MHz
  // so MSEL = 19 for 512Mhz processor clock
  //**************************************
  //*** WARNING!! 
  //* setting this too high will fry the DSP!
  *pPLL_CTL = 0x2600;
  //*************************************
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

