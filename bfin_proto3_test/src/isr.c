#include "bfin_core.h"

#include "init.h"
#include "isr.h"
#include "types.h"
#include "util.h"

/// external data, initialized here:
//volatile u8 testTxBuf[4] = { 0, 1, 2, 3 };

// some arbitrary data for the 12 DAC channels
// static u16 data[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// channel counter (0-3)
// static u8 chan = 0;


/*
static inline void fillDacBuf(u8 dacIdx) {
  sport1TxBuf.dacs[0].com |= chan;  
  sport1TxBuf.dacs[0].dat = data[dacIdx * 4 + chan];
  byte_swap( (u8*)&(sport1TxBuf.dacs[0].dat), 4 );
}
*/

#if 0
void sport1_tx_isr() {
  u8 i;
  // confirm interrupt handling
  //  *pDMA4_IRQ_STATUS = 0x0001;
  
  /*
  if (chan == 0) {
    // update the data 
    ///// test: saw waves at various frequencies
    for(i=0; i<12; i++) {
      data[i] += (i << 3);
    }
  }

  // increment channel counter
  chan = (chan + 1) % 4;
  // fill the tx buffer
  fillDacBuf(0);
  fillDacBuf(1);
  fillDacBuf(2);
  fillDacBuf(3);
  */
  /*
  for(i=0; i<9; i++) {
    sport1TxBuf[i] += ((i+1) * 4);
  }
  */

  for(i=0; i<4; i++) {
    testTxBuf[i] += ((i+1) * 4);
  }
  *pDMA4_IRQ_STATUS = 0x0001;
}


//// DEBUG: error handler
void error_isr() {
  u8 dummy = 0;
  u32 irqStatus;
  irqStatus = *pSIC_ISR;
  dummy++;
  dummy++;
}
#endif


// spi receive interrupt (from avr32)
void spi_rx_isr() {
  *pSPI_TDBR = spi_process(*pSPI_RDBR);
}
