// bfin toolchain
#include <blackfin.h>
#include <cdefBF533.h>  
#include "ccblkfn.h"

#include "serial.h"


// SPORT0 word length
#define SLEN_24	0x0017

//--------------------------------------------------------------------//
// init_spi_slave()
// (re-)configure spi in slave mode to receive control data from avr32 
// FIXME: using pingpong DMA could halve the number of interrupts
void init_spi_slave(void) {
  int j;
  // don't attempt to drive the clock
  *pSPI_BAUD = 0;
  // reset the flags register? to defaults?
  *pSPI_FLG = 0xff00;

  // try clearing the rx error bit? (sticky - W1C)
  *pSPI_STAT |= 0x10;  
  *pSPI_STAT |= 0x10;  

  // slave mode, 
  // 8 bit transfers (MSB first, non-dma rx mode, 
  // overwrite (interrupt when SPI_RDBR is full),
  // phase: seems crazy but bfin and avr32 have opposite definitions of clock phase!
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
  // 24-bit data, Secondary side enable, Stereo frame sync enable
  *pSPORT0_RCR1 = RFSR | RCKFE;
  *pSPORT0_RCR2 = SLEN_24 | RXSE | RSFSE;
  
  // Sport0 transmit configuration
  // External CLK, External Frame sync, MSB first, Active Low
  // 24-bit data, Secondary side enable, Stereo frame sync enable
  *pSPORT0_TCR1 = TFSR | TCKFE | LATFS;
  *pSPORT0_TCR2 = SLEN_24 | TXSE | TSFSE;
}

// configure sport1  [ drive 1x AD5686 from DTSEC ]
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
 
  //// secondary side enabled : TXSE  = 1
  ///// 24-bit word length
  //     *pSPORT1_TCR2 = 23 | TXSE ;
  //// 25-bit cause DACs need an extra cycle to recover, ugggh
  *pSPORT1_TCR2 = 24 | TXSE ;
  // tclk = sclk / ( 2 x (div + 1)
  /// DAC datasheet indicates we can go up to 50Mhz
  // here's 27 Mhz
  *pSPORT1_TCLKDIV = 1;

}
