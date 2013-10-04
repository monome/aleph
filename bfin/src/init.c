
#include "bfin_core.h"
#include "gpio.h"
#include "isr.h"
#include "init.h"

// #define ADD_DACS 1

//------ global variables initialized here

// control registers and associated values for AD1939
/*
static volatile short codec1939TxRegs[CODEC_1939_NUM_REGS][2] = {
  {  PLL_CLOCK_0        , 0x80 } , // enable, xtal->master->PLL
  {  PLL_CLOCK_1        , 0x00 } ,
  {  DAC_CONTROL_0      , 0x00 } ,
  {  DAC_CONTROL_1      , 0x00 } ,
  {  DAC_CONTROL_2      , 0x00 } ,
  {  DAC_MUTE           , 0x00 } ,
  {  DAC_L1_VOLUME      , 0x00 } ,
  {  DAC_R1_VOLUME      , 0x00 } ,
  {  DAC_L2_VOLUME      , 0x00 } ,
  {  DAC_R2_VOLUME      , 0x00 } ,
  {  DAC_L3_VOLUME      , 0x00 } ,
  {  DAC_R3_VOLUME      , 0x00 } ,
  {  DAC_L4_VOLUME      , 0x00 } ,
  {  DAC_R4_VOLUME      , 0x00 } ,
  {  ADC_CONTROL_0      , 0x00 } ,
  {  ADC_CONTROL_1      , 0x00 } ,
  /// ADC master, abclk
  {  ADC_CONTROL_2      , 0b01000000 }
  //  {  ADC_CONTROL_2      , 0x00 }
};
*/

// SPORT0 DMA transmit buffer
volatile s32 iTxBuf[4];
// SPORT0 DMA receive buffer
volatile s32 iRxBuf[4];

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
  *pEBIU_SDBCTL = 0x0025; //1.7	64 MB bank size
  //	*pEBIU_SDBCTL = 0x0013;	//1.6 and below 32 MB
  
  //SDRAM Memory Global Control Register
  *pEBIU_SDGCTL = 0x0091998d;//0x998D0491;
  ssync();
}

/* void init_flash(void) { */
/*   // flash A, port A0 -> AD1836_reset */
/*   //  *pFlashA_PortA_Dir = 0x1; */
/*   // flash A, ports [B0, B5] -> [led1, led6] */
/*   //  *pFlashA_PortB_Dir = 0x3f; */
/* } */

/* static void spi_send_byte(u8 ch) { */
/*   *pSPI_TDBR = ch; */
/*   ssync(); */

/*   while( (*pSPI_STAT & TXS)  != 0 ) { ;; } */
/*   while( (*pSPI_STAT & SPIF) == 0 ) { ;; } */
/* } */

 // setup SPI0 -> AD1939 config */
void init_1939(void) { 
  //  u8 i; 
  u32 del;


  //// reset codec
  //  *pFIO_FLAG_D &= CODEC_RESET_MASK;
  CODEC_RESET_LO;
  del = 100; while(del--) { ;; } 
  CODEC_RESET_HI;
  //  *pFIO_FLAG_D |= (0xffff ^ CODEC_RESET_MASK);
  return;

  /// using the codec in standalone now, dont need SPI config

  /* //// TESTING: */
  /* /// wait for the codec to reset */
  /* /// from the datasheet, this could take an absolute maximum of (4096 / 6.9Mhz)s (?) */
  /* /// we are using BF clock = 513 MHz, so: */
  /* del = 308000; while(del--) { ;; }  */

  /* //  set PF4 as slave select */
  /* *pSPI_FLG = 0xFF10; */

  /* // Set baud rate SCK = HCLK/(2*SPIBAUD) SCK = 2MHz */
  /* *pSPI_BAUD = 16; */

  /* // setup SPI: */
  /* /// master, non-DMA mode, 8b transfers, MSB first, */
  /* /// TX on buffer write, clock rising edge: */

  /* //// this would give us the waveform we want but puts CS under hardware control */
  /* //  *pSPI_CTL = MSTR | TIMOD_BUF_TX; */

  /* /// SPI controller can't do 24-bits transfers-- */
  /* /// workaround: 8-bit transfers and manual slave select */
  /* // ( see http://ez.analog.com/thread/1248 */
  /* //  SS from software : */
  /* *pSPI_CTL = MSTR | CPHA | CPOL | TIMOD_BUF_TX; */

  /* // enable SPI */
  /* *pSPI_CTL = (*pSPI_CTL | SPE); */

  /* ssync(); */
  /* // loop over registers */
  /* for(i=0; i<CODEC_1939_NUM_REGS; i++) { */
  /*   // select slave */
  /*   //    *pFIO_FLAG_D &= CODEC_SS_MASK; */

  /*   // bring SS low (PF4) */
  /*   *pSPI_FLG = 0xef10; */

  /*   // send command byte */
  /*   spi_send_byte(CODEC_CMD_BYTE); */
  /*   // send register byte */
  /*   spi_send_byte(codec1939TxRegs[i][0]); */
  /*   /// send dta byte */
  /*   spi_send_byte(codec1939TxRegs[i][1]); */
    
  /*   // bring SS high (PF4) */
  /*   *pSPI_FLG = 0xff10; */
  /*   } */
} // init_1939


//--------------------------------------------------------------------//
// init_spi_slave()
// (re-)configure spi in slave mode to receive control data from avr32 
void init_spi_slave(void) {
  int j;
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
  *pSPI_CTL = CPHA | GM | SZ;
  // enable transmit on MISO
  *pSPI_CTL |= EMISO;
  // enable spi (now in slave mode)
  *pSPI_CTL = (*pSPI_CTL | SPE);
  // clear the spi rx register by reading from it
  j = *pSPI_RDBR;
  // clear the rx error bit (sticky - W1C)
  *pSPI_STAT |= 0x10;
}

// configure sport0 for i2s mode with external clock
void init_sport0(void)
{
  // Sport0 receive configuration
  // External CLK, External Frame sync, MSB first, Active Low
  // 24-bit data, Stereo frame sync enable
  *pSPORT0_RCR1 = RFSR | RCKFE;
  *pSPORT0_RCR2 = SLEN_24 | RXSE | RSFSE;
  
  // Sport0 transmit configuration
  // External CLK, External Frame sync, MSB first, Active Low
  // 24-bit data, Secondary side enable, Stereo frame sync enable
  *pSPORT0_TCR1 = TFSR | TCKFE;
  *pSPORT0_TCR2 = SLEN_24 | TXSE | TSFSE;
}

// CONFIGURE sport1  [ to drive 3x AD5684 from DT1PRI ]
void init_sport1(void) {
  
  //----- note: edge selection is for *driving* the pins, sampled opposite
  //// TFS/clk driven w/ rising edge : TCKFE  = 0
  //// early frame sync              : LATFS  = 0
  //// TFS active high               : LTFS   = 0
  //// data-dependent TFS            : DITFS  = 0
  //// internal clock                : ITCLK  = 1
  //// internal TFS                  : ITFS   = 1
  //// frame sync required           : TFSR  = 1
  //// no companding                 : TDTYPE = 00
  //// MSB first                     : TLSBIT = 0  
  *pSPORT1_TCR1 = ITCLK | ITFS | TFSR;
  
//// normal mode             : TSFSE = 0
  //// secondary side enabled : TXSE  = 1
  ///// 24-bit word length
  //     *pSPORT1_TCR2 = 23 | TXSE ;
  //// 25-bit cause DACs need an extra cycle to recover, ugggh
  *pSPORT1_TCR2 = 24 | TXSE ;

  // clock division: we want ~10Mhz, core clock is 108Mhz
  // tclk = sclk / ( 2 x (div + 1)
  //    *pSPORT1_TCLKDIV = 5;
  //// slower:
  *pSPORT1_TCLKDIV = 100;
  //// slowest:
  //  *pSPORT1_TCLKDIV = 0xffff;
  
  /// enable sport1
  ////// do this later for DMA
  ///    *pSPORT1_TCR1 |= TSPEN;

  /// receive configuration: don't care?
  //  *pSPORT1_RCR1 = RFSR | RCKFE;
  //  *pSPORT1_RCR2 = SLEN_24 | RXSE | RSFSE;
}


// initialize DMA in autobuffer mode
void init_DMA(void) {
  // Set up DMA1 to receive
  // Map DMA1 to Sport0 RX
  *pDMA1_PERIPHERAL_MAP = 0x1000;
  
  // Configure DMA1
  // 32-bit transfers, Interrupt on completion, Autobuffer mode
  *pDMA1_CONFIG = WNR | WDSIZE_32 | DI_EN | FLOW_1;
  // Start address of data buffer
  *pDMA1_START_ADDR = (void *)iRxBuf;
  // DMA inner loop count
  *pDMA1_X_COUNT = 4;
  // Inner loop address increment
  *pDMA1_X_MODIFY = 4;
  
  // Set up DMA2 to transmit
  // Map DMA2 to Sport0 TX
  *pDMA2_PERIPHERAL_MAP = 0x2000;
  
  // Configure DMA2
  // 32-bit transfers, Autobuffer mode
  *pDMA2_CONFIG = WDSIZE_32 | FLOW_1;
  // Start address of data buffer
  *pDMA2_START_ADDR = (void *)iTxBuf;
  // DMA inner loop count
  *pDMA2_X_COUNT = 4;
  // Inner loop address increment
  *pDMA2_X_MODIFY = 4;

  /// DMA4: transmit
  /// map dma4 to sport1 tx
  *pDMA4_PERIPHERAL_MAP = 0x4000;
  // configure DMA4
  *pDMA4_CONFIG = WDSIZE_32 | FLOW_1;
  //*pDMA4_CONFIG = WDSIZE_32 | FLOW_1 | DI_EN;
  // Start address of data buffer
  *pDMA4_START_ADDR = (void *)(&cvTxBuf);
  // DMA inner loop count
  *pDMA4_X_COUNT = 1;
  // Inner loop address increment
  *pDMA4_X_MODIFY = 4;
}

// enable sport0 DMA
void enable_DMA_sport0(void) {
  // enable DMAs
  *pDMA2_CONFIG	= (*pDMA2_CONFIG | DMAEN);
  *pDMA1_CONFIG	= (*pDMA1_CONFIG | DMAEN);
  
  // enable Sport0 TX and RX
  *pSPORT0_TCR1 	= (*pSPORT0_TCR1 | TSPEN);
  *pSPORT0_RCR1 	= (*pSPORT0_RCR1 | RSPEN);
}

// begin transfers with sport1 and dma4
void enable_DMA_sport1(void) {
  // enable DMAs
  *pDMA4_CONFIG	= (*pDMA4_CONFIG | DMAEN);
  // enable sport1 tx
  *pSPORT1_TCR1 	= (*pSPORT1_TCR1 | TSPEN);
  //  *pSPORT1_RCR1 	= (*pSPORT1_RCR1 | RSPEN); 
}


// initialize programmable flags for button input
void init_flags(void) {
  // outputs
  *pFIO_DIR = LED3_UNMASK | LED4_UNMASK | BUSY_UNMASK | DAC_RESET_UNMASK;

  /// no gpio inputs.
  /*
  // inputs 
    *pFIO_INEN = 0;
  // edge-sensitive
  *pFIO_EDGE = 0x0f00;
  // both rise and fall
  *pFIO_BOTH = 0x0f00;
  // set interrupt mask
  *pFIO_MASKA_D = 0x0f00;
  */
}

// assign interrupts
void init_interrupts(void) {
  int i=0;

  //// FIXME: should try to get sport0 interrupt on EVT

  // no dacs (only sport0 and spi) :
  //  *pSIC_IAR1 = 0xff3fff2f;
  //  *pSIC_IAR2 = 0xffffffff;
  
  // assign core IDs to peripheral interrupts:

  // no errors (fixme?)
  *pSIC_IAR0 = 0xffffffff;

    *pSIC_IAR1 = 0xff32ff1f;	// sport0 rx -> IVG8 (CID 1), 
				// sport1 tx -> IVG9 (CID 2), 
                           	// spi rx -> IVG10 (CID 3),
    
    // no codec:
    //*pSIC_IAR1 = 0xff32ffff;

  *pSIC_IAR2 = 0xffffffff;

  // assign ISRs to interrupt vectors:
  //  *pEVT9 = sport0_rx_isr;
  //  *pEVT10 = spi_rx_isr;
  //  *pEVT11 = sport1_tx_isr;

  *pEVT8 = sport0_rx_isr;
  *pEVT9 = sport1_tx_isr;  
  *pEVT10 = spi_rx_isr;


  // unmask in the core event processor
    //  asm volatile ("cli %0; bitset (%0, 9); bitset(%0, 10); bitset(%0, 11); sti %0; csync;": "+d"(i));
  //  asm volatile ("cli %0; bitset (%0, 9); bitset(%0, 10); sti %0; csync;": "+d"(i));
  asm volatile ("cli %0; bitset (%0, 8); bitset (%0, 9); bitset(%0, 10); sti %0; csync;": "+d"(i));

  // unmask in the peripheral interrupt controller
  *pSIC_IMASK = 0x00003200;
  // no codec:
  //  *pSIC_IMASK = 0x00003000;
}
