
#include "bfin_core.h"
#include "isr.h"
#include "init.h"


//----- function definitions
// initialize clocks
void init_clocks(void) {
  //// check:
  //  const u16 pll_ctl = *pPLL_CTL;
  //  const u16 pll_div = *pPLL_DIV;
  //  u8 dum = 0;
  //  dum++;
  //  dum++;

  // set MSEL = 20 for core clock of 108Mhz
//  *pPLL_CTL = 0x2800;
/////// changing to 533Mhz part
/// MSEL = 19
  //// VCO = 19 * CLKIN = 513
  *pPLL_CTL = 0x2600;
  ssync();
}


// initialize EBI
void init_EBIU(void) {
  //  straight from the self-test example project
  // Initalize EBIU control registers to enable all banks	
  *pEBIU_AMBCTL1 = 0xFFFFFF02;
  ssync();
  
  // -- not sure why there is a read here, possibly anomaly 05000157?
  //temp = *pEBIU_AMBCTL1;
  //temp++;
  
  *pEBIU_AMGCTL = 0x00FF;
  ssync();
  
  // Check if already enabled
  if( SDRS != ((*pEBIU_SDSTAT) & SDRS) ) {
    return;
  }
  
  //SDRAM Refresh Rate Control Register
  //  *pEBIU_SDRRC = 0x01A0; 
  // for 108Mhz system clock:
  *pEBIU_SDRRC = 835;
  
  //SDRAM Memory Bank Control Register
  *pEBIU_SDBCTL = 0x0025; //1.7	64 MB
  //	*pEBIU_SDBCTL = 0x0013;	//1.6 and below 32 MB
  
  //SDRAM Memory Global Control Register	
  *pEBIU_SDGCTL = 0x0091998d; //0x998D0491;
  ssync();
}


void init_flags(void) {
  // LED outputs on pf14, pf15
  *pFIO_DIR = 0xc000;
  /*
  // configure pf08-pf11 for input
  *pFIO_INEN = 0x0f00;
  *pFIO_DIR = 0x0000;
  // edge-sensitive
  *pFIO_EDGE = 0x0f00;
  // both rise and fall
  *pFIO_BOTH = 0x0f00;
  // set interrupt mask
  *pFIO_MASKA_D = 0x0f00;
  */
}


//--------------------------------------------------------------------//
// init_spi_slave()
// (re-)configure spi in slave mode to receive control data from avr32 
void init_spi_slave(void) {
  int j;
  int spi_stat_debug;
 
  // don't attempt to drive the clock
  *pSPI_BAUD = 0;
  // reset the flags register? to defaults?
  *pSPI_FLG = 0xff00;

  // try clearing the rx error bit? (sticky - W1C)
  *pSPI_STAT |= 0x10;  
  *pSPI_STAT |= 0x10;  

  // slave mode, 16 bit transfers, MSB first, non-dma rx mode, overwrite (interrupt when SPI_RDBR is full) 
  // phase: seems crazy but i think bfin and avr32 have opposite definitions of clock phase! oh lordy
  //  *pSPI_CTL = CPHA | SIZE | GM;

  // actually need 8 bits to be compatible with SPI-boot
  //----->>>>>  *pSPI_CTL = CPHA | GM;

  // TEST: send zeros after TDBR is emptied
  *pSPI_CTL = CPHA | GM | SZ;

  // enable transmit on MISO
  *pSPI_CTL |= EMISO;
  spi_stat_debug = *pSPI_STAT;
  // enable spi in slave mode
  *pSPI_CTL = (*pSPI_CTL | SPE);
  spi_stat_debug = *pSPI_STAT;
  // clear the spi rx register by reading from it
  j = *pSPI_RDBR;
  // clear the rx error bit (sticky - W1C)
  *pSPI_STAT |= 0x10;
}


//
// assign interrupts
void init_interrupts(void) {
  int i=0;
  // assign core IDs to peripheral interrupts:
  *pSIC_IAR0 = 0xffffffff;
  // spi rx           -> ID3 = IVG10
  *pSIC_IAR1 = 0xff3fffff;
  *pSIC_IAR2 = 0xffffffff;
  
  // assign ISRs to interrupt vectors:
  *pEVT10 = spi_rx_isr;

  asm volatile ("cli %0; bitset (%0, 10); sti %0; csync;": "+d"(i));
  // unmask spi interrupt in PIC
  *pSIC_IMASK = 0x00002000;
}


/*
void init_timers(void) {
  *pTIMER0_CONFIG		= 0x0019;
  *pTIMER0_PERIOD		= 0x00800000;
  *pTIMER0_WIDTH		= 0x00400000;
  *pTIMER_ENABLE		= 0x0001;
}
*/
