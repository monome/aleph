
#include "bfin_core.h"
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
  *pFIO_DIR = (1 << LED0_PIN) |  (1 << LED1_PIN) |  (1 << DAC_RESET_PIN);
}

// CONFIGURE sport1  [ to drive 3x AD5684 from DT1PRI ]
void init_sport1(void) {
  //  const u32 slen_32 = 0x1f;
  u32 config;

  /*
  *pSPORT0_TFSDIV = 0x001F;      // frame sync every 32 clk cycles
  *pSPORT0_TCLKDIV =0x0002;
  *pSPORT0_TCR2 = SLEN;    // SLEN is 0F
  *pSPORT0_TCR1 = 0x7602;
  *pSPORT0_TCR1 = 0x7603; 
  */

  // setup sport1 control register:
  //// TFS and clock on falling edge : TCKFE  = 1
  //// early frame sync              : LATFS  = 0
  //// TFS active high               : LTFS   = 0
  //// data-dependent TFS            : DITFS  = 0
  //// internal clock                : ITCLK  = 1
  //// internal TFS                  : ITFS   = 1
  //// no companding                 : TDTYPE = 00
  //// MSB first                     : TLSBIT = 0  
  //  *pSPORT1_TCR1 = TCKFE | ITCLK | ITFS;
  

  //// TFS and clock on falling edge : TCKFE  = 1
  //// early frame sync              : LATFS  = 0
  //// TFS active high               : LTFS   = 0
  //// data-dependent TFS            : DITFS  = 0
  //// internal clock                : ITCLK  = 1
  //// internal TFS                  : ITFS   = 1
  //// frame sync required           : TFSR  = 1
  //// no companding                 : TDTYPE = 00
  //// MSB first                     : TLSBIT = 0  
  //    *pSPORT1_TCR1 = TCKFE | ITCLK | ITFS | TFSR;
  *pSPORT1_TCR1 = ITCLK | ITFS | TFSR;
  config = *pSPORT1_TCR1;

  //===== TEST: data-independent TFS
  //  *pSPORT1_TCR1 = TCKFE | ITCLK | ITFS | DITFS;
 
  //// normal mode             : TSFSE = 0
  //// secondary side enabled : TXSE  = 1
  ///// 24-bit word length
  //  *pSPORT1_TCR2 = 23 | TXSE ;
  //// test: 25-but cause DACs need an extra cycle to recover
  *pSPORT1_TCR2 = 24 | TXSE ;

  /// TEST: 32-bit word length
  //  *pSPORT1_TCR2 = slen_32;

  // clock division: we want ~10Mhz, core clock is 108Mhz
  // tclk = sclk / ( 2 x (div + 1)
    *pSPORT1_TCLKDIV = 5;
  //// slower:
  //  *pSPORT1_TCLKDIV = 23;
  //// slowest:
  //  *pSPORT1_TCLKDIV = 0xffff;

  // we want frame syncs every 24 clocks,
  // FS period = clk period * (TFSDIV + 1)

  //==== TEST: frankly i don't really understand the implications of this.
  //  *pSPORT1_TFSDIV = 23;

  /// enable sport1
  ////// do this later for DMA
  *pSPORT1_TCR1 |= TSPEN;

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
  //  *pSIC_IAR0 = 0xfff0ff0f ;

  // sport1 rx (dma4) = IVG9, core ID 2
  //  *pSIC_IAR1 = 0xfff2ffff;
  
  /// only timer:
  *pSIC_IAR0 = 0xffffffff;
  *pSIC_IAR1 = 0xffffffff;
  // timer0 = IVG11, core ID 4
  *pSIC_IAR2 = 0xfffffff4; 
  
  *pEVT11 = timer0_isr;
  //  *pEVT9 = sport1_tx_isr;
  //  *pEVT7 = error_isr;
  //  asm volatile ("cli %0; bitset (%0, 9); sti %0; csync;": "+d"(i));
  //  asm volatile ("cli %0; bitset (%0, 11); bitset (%0, 9); bitset(%0, 7); sti %0; csync;": "+d"(i));
  asm volatile ("cli %0; bitset (%0, 11); sti %0; csync;": "+d"(i));
  
  // enable timer0 interrupt
  *pSIC_IMASK = 0x00010000;
  // enable interrupts on sport1 tx (IVG9)
  //  *pSIC_IMASK = 0x00001000;
  /// debug: error handlers
  //  *pSIC_IMASK = 0x0000100a;
} 

void init_timers(void) {
  *pTIMER0_CONFIG		= 0x0019;
  //  *pTIMER0_PERIOD		= 0x00080000;
  //  *pTIMER0_WIDTH		= 0x00040000;
  *pTIMER0_PERIOD		= 0x00000800;
  *pTIMER0_WIDTH		= 0x00000400;
  *pTIMER_ENABLE		= 0x0001;
}
