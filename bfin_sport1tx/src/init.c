
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
  *pLL_CTL = 0x2600;
  ssync();
  
}


// configure sport1 to drive 3x AD5684 from DT1PRI
void init_sport1(void) {
  //  const u32 slen_24 = 0x17;
  const u32 slen_32 = 0x1f;
  // setup sport1 control register:
  //// TFS and clock on falling edge : TCKFE  = 1
  //// early frame sync              : LATFS  = 0
  //// TFS active high               : LTFS   = 0
  //// data-dependent TFS            : DITFS  = 0
  //// internal clock                : ITCLK  = 1
  //// internal TFS                  : ITFS   = 1
  //// no companding                 : TDTYPE = 00
  //// MSB first                     : TLSBIT = 0  
  *pSPORT1_TCR1 = TCKFE | ITCLK | ITFS;
  //===== TEST: data-independent TFS
  //  *pSPORT1_TCR1 = TCKFE | ITCLK | ITFS | DITFS;
 
  //// normal mode             : TSFSE = 0
  //// secondary side disabled : TXSE  = 0
  ///// 24-bit word length
  //  *pSPORT1_TCR2 = slen_24;

  /// TEST: 32-bit word length
  *pSPORT1_TCR2 = slen_32;

  // clock division: we want ~10Mhz, core clock is 108Mhz
  // tclk = sclk / ( 2 x (div + 1)
  *pSPORT1_TCLKDIV = 5;
  // we want frame syncs every 24 clocks,
  // FS period = clk period * (TFSDIV + 1)

  //==== TEST: frankly i don't really understand the implications of this.
  //  *pSPORT1_TFSDIV = 23;

  /// enable sport1
  ///// - do this later
  //  *pSPORT1_TCR1 |= TSPEN;

  /// receive configuration: don't care?
  //  *pSPORT1_RCR1 = RFSR | RCKFE;
  //  *pSPORT1_RCR2 = SLEN_24 | RXSE | RSFSE;

}

// initialize DMA in autobuffer mode
void init_dma(void) {
  const u32 flow_1    = 0x1000;
  //  const u32 wdsize_32 = 
  // map DMA4 to Sport1 TX, memory DMA
  *pDMA4_PERIPHERAL_MAP = 0x4000;
  // configure DMA4:
  //// autobuffer mode          : FLOW    = 001
  //// data interrupt enabled   : DI_EN   = 1
  //// interrupt on completion  : DI_SEL  = 0
  //// DMA is a memory write    : WNR     = 1
  //// one-dimensional          : DMA2D   = 0
  //// retain FIFO between work : RESTART = 0
  //// 8-bit transfers          : WDSIZE  = 0
  //  *pDMA4_CONFIG = flow_1 | DI_EN | WNR;
  //===== TEST: 32-bit transfers ??
  *pDMA4_CONFIG = flow_1 | DI_EN | WNR | WDSIZE_32;
  // address of data buffer:
  //  *pDMA4_START_ADDR = (void*)(&sport1TxBuf);
 

 /*
  *pDMA4_START_ADDR = (void*)(sport1TxBuf);
  
  
// inner loop count
    *pDMA4_X_COUNT = 9;
  // inner loop address increment (1 byte)
    *pDMA4_X_MODIFY = 1;
    */

  // ===  TEST:
  *pDMA4_START_ADDR = (void*)(testTxBuf);
  // inner loop count
  *pDMA4_X_COUNT = 1;
  // inner loop address increment (1 byte)
  *pDMA4_X_MODIFY = 4;
  
 

}

// assign interrupts
void init_interrupts(void) {
  int i=0;

  // assign core IDs to peripheral interrupts:
  // DEBUG: enable DMA and SPORT1 error interrupts
  // SPORT1 Error Interrupt PID:4 IVG7  CID:0
  // DMA Error (generic)    PID:1 IVG7  CID:0

  //  *pSIC_IAR0 = 0xffffffff;
  *pSIC_IAR0 = 0xfff0ff0f ;

  // sport1 rx (dma4) = IVG9, core ID 2
  *pSIC_IAR1 = 0xfff2ffff;
  *pSIC_IAR2 = 0xffffffff;

  *pEVT9 = sport1_tx_isr;
  *pEVT7 = error_isr;
  //  asm volatile ("cli %0; bitset (%0, 9); sti %0; csync;": "+d"(i));
  asm volatile ("cli %0; bitset (%0, 9); bitset(%0, 7); sti %0; csync;": "+d"(i));
  
  // enable interrupts on sport1 tx (IVG9)
  //  *pSIC_IMASK = 0x00001000;
  /// debug: error handlers
  *pSIC_IMASK = 0x0000100a;
}


/*
void init_timers(void) {
  *pTIMER0_CONFIG		= 0x0019;
  *pTIMER0_PERIOD		= 0x00800000;
  *pTIMER0_WIDTH		= 0x00400000;
  *pTIMER_ENABLE		= 0x0001;
}
*/
