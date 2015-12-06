
#include "init.h"
#include "gpio.h"

void init_clock(void) { 
  // VCO = MSEL * CLKIN
  // ccore clock is 108MHz
  // so MSEL = 19 for 512Mhz processor clock
  //! >>> BEWARE! <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  //! >>> changing this value can definitely fry the blackfin!
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

