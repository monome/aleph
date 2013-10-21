
#include "bfin_core.h"
#include "gpio.h"
#include "isr.h"
#include "init.h"


//----- function definitions
// initialize clocks
void init_clocks(void) {
  // part is 533Mhz max
  // CLKIN = 27.5 Mhz
  // want VCO = 513Mhz = 19 * CLKIN 
  // so MSEL = 19
  *pPLL_CTL = 0x2600;
  ssync(); 
}

// ini flags
void init_flags(void) {
  //  *pFIO_DIR = 0xc000;
  *pFIO_DIR = (1 << LED0_PIN) |  (1 << LED1_PIN) |  (1 << DAC_RESET_PIN) | LDAC_UNMASK;
}

// CONFIGURE sport1  [ to drive 3x AD5684 from DT1PRI ]
void init_sport1(void) {
  u32 config;

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
  

  //----- note: edge selection is for *driving* the pins, sampled opposite
  //// TFS/clk driven w/ rising edge : TCKFE  = 0
  //// late frame sync              : LATFS  = 1
  //// TFS active low               : LTFS   = 1
  //// data-dependent TFS            : DITFS  = 0
  //// internal clock                : ITCLK  = 1
  //// internal TFS                  : ITFS   = 1
  //// frame sync required           : TFSR  = 1
  //// no companding                 : TDTYPE = 00
  //// MSB first                     : TLSBIT = 0  
  // *pSPORT1_TCR1 = ITCLK | ITFS | TFSR | LTFS | LATFS;
  


  //===== TEST: data-independent TFS
  //  *pSPORT1_TCR1 = ITCLK | ITFS | DITFS;
 
  //// normal mode             : TSFSE = 0
  //// secondary side enabled : TXSE  = 1
  ///// 24-bit word length
    //     *pSPORT1_TCR2 = 23 | TXSE ;
  //// test: 25-bit cause DACs need an extra cycle to recover, ugggh
    *pSPORT1_TCR2 = 24 | TXSE ;

  /// TEST: 32-bit word length
  //  *pSPORT1_TCR2 = slen_32;

  // clock division: we want ~10Mhz, core clock is 108Mhz
  // tclk = sclk / ( 2 x (div + 1)
    //    *pSPORT1_TCLKDIV = 5;
    /// part should be capable of 50 Mhz...
    *pSPORT1_TCLKDIV = 1;
    //// slower:
    //  *pSPORT1_TCLKDIV = 100;
  //// slowest:
  //  *pSPORT1_TCLKDIV = 0xffff;

  // we want frame syncs every 24 clocks,
  // FS period = clk period * (TFSDIV + 1)
    //// need an extra bit at the end for the DAC, grr
    //    *pSPORT1_TFSDIV = 23;
	
    //// daisychain x2:
    //  *pSPORT1_TFSDIV = 48;
    
    config = *pSPORT1_TCR1;
    
    /// enable sport1
  ////// do this later for DMA
    ///    *pSPORT1_TCR1 |= TSPEN;

  /// receive configuration: don't care?
  //  *pSPORT1_RCR1 = RFSR | RCKFE;
  //  *pSPORT1_RCR2 = SLEN_24 | RXSE | RSFSE;

}

// assign interrupts
void init_interrupts(void) {
  int i=0;

  // assign core IDs to peripheral interrupts:
  // DEBUG: enable DMA and SPORT1 error interrupts
  //  SPORT1 Error Interrupt PID:4 IVG7  CID:0
  // DMA Error (generic)    PID:1 IVG7  CID:0
  *pSIC_IAR0 = 0xfff0ff0f ;
  //  *pSIC_IAR0 = 0xffffffff;
  // sport1 tx (dma4) = IVG9, core ID 2
  *pSIC_IAR1 = 0xfff2ffff;
  // timer0 = IVG11, core ID 4
  *pSIC_IAR2 = 0xfffffff4; 
  
  *pEVT11 = timer0_isr;
  *pEVT9 = sport1_tx_isr;
  *pEVT7 = error_isr;
  //  asm volatile ("cli %0; bitset (%0, 9); sti %0; csync;": "+d"(i));
    asm volatile ("cli %0; bitset (%0, 11); bitset (%0, 9); bitset(%0, 7); sti %0; csync;": "+d"(i));

    //asm volatile ("cli %0; bitset (%0, 11); bitset (%0, 9); sti %0; csync;": "+d"(i));
  
  // enable interrupts on timer0, sport1_tx (dma4), errors
  *pSIC_IMASK = 0x0001100a;
  /// debug: error handlers
  //  *pSIC_IMASK = 0x0000100a;
} 


#define FLOW_1 	0x1000
void init_dma(void) {
  /// map dma4 to sport1 tx
  *pDMA4_PERIPHERAL_MAP = 0x4000;
  // configure DMA4
  /// no interrupt...
  *pDMA4_CONFIG = WDSIZE_32 | FLOW_1;
  //*pDMA4_CONFIG = WDSIZE_32 | FLOW_1 | DI_EN;
  // Start address of data buffer
  *pDMA4_START_ADDR = (void *)(&txBuf);
  // DMA inner loop count
  *pDMA4_X_COUNT = 1;
  // Inner loop address increment
  *pDMA4_X_MODIFY = 4;
}


// begin transfers with sport1 and dma4
void enable_sport1_dma(void) {
  // enable DMAs
  *pDMA4_CONFIG	= (*pDMA4_CONFIG | DMAEN);
  // enable sport1 tx
  *pSPORT1_TCR1 	= (*pSPORT1_TCR1 | TSPEN);
  //  *pSPORT1_RCR1 	= (*pSPORT1_RCR1 | RSPEN); 
}

void init_timers(void) {
  *pTIMER0_CONFIG		= 0x0019;
    *pTIMER0_PERIOD		= 0x00000100;
    *pTIMER0_WIDTH		= 0x00000080;
  //  *pTIMER0_PERIOD		= 0x00000020;
  //  *pTIMER0_WIDTH		= 0x00000010;
  *pTIMER_ENABLE		= 0x0001;
}
