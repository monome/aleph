#include "bfin_core.h"

#include "init.h"
#include "isr.h"
#include "types.h"
#include "util.h"

/// external data, initialized here:
//volatile u8 testTxBuf[4] = { 0, 1, 2, 3 };

// some arbitrary data for the 12 DAC channels
//static u16 data[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// channel counter (0-3)
//static u8 chan = 0;

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

static u32 chanIncMul = 4;
static u32 chanIncOff = 1;

static u16 dacData[4] = { 0, 0, 0, 0 };
//static u16 dacData[4] = { 0xcccc, 0xdddd, 0xeeee, 0xffff };
static u32 txBuf = 0;

static u8 ledFlag = 0;

static void sport1_tx(u32* data) {
  /*
  u32 stat = *pSPORT1_STAT;
  int dum;
  if (stat & TXHRE) {
    dum = 0;
    dum++;
  }
  */
  // wait till buffer becomes empty
  while((*pSPORT1_STAT & TXHRE) == 0) {;;}  
  // copy to tx buffer
  // need 2 writes to fill the FIFO i guess??
  *pSPORT1_TX32 = (*data) << 1;
  *pSPORT1_TX32 = (*data) << 1;
  //  *pSPORT1_TX32 = *data;
  //  *pSPORT1_TX32 = *data;
  ///// uhhh
  //  stat = *pSPORT1_STAT;
}

static void update_channel(int ch) {
  dacData[ch] += (ch * chanIncMul + chanIncOff);
  txBuf = 0;
  txBuf |= (DAC_COM_WRITE << DAC_COM_LSHIFT);
  txBuf |= ((1 << ch) << DAC_ADDR_LSHIFT);
  txBuf |= dacData[ch];
  //  sport1_tx(&txBuf);
  // test: need lshift cause of 25/24bit discrepancy
  sport1_tx(&txBuf);
}

static void update_dummy(void) {
  static const u32 dummy = 0;
  sport1_tx(&dummy);
}


void timer0_isr() {
  // handle
  *pTIMER_STATUS = 0x0001;
  // random leds
  if(ledFlag) {
    *pFIO_FLAG_D &= 0xbfff;
    ledFlag = 0;
  } else {
    *pFIO_FLAG_D |= 0x4000;
    ledFlag = 1;
  }
  // dac
  update_channel(0);
  update_channel(1);
  update_channel(2);
  update_channel(3);
  update_dummy();
}

#if 0
/*
static inline void fillDacBuf(u8 dacIdx) {
  sport1TxBuf.dacs[0].com |= chan;  
  sport1TxBuf.dacs[0].dat = data[dacIdx * 4 + chan];
  byte_swap( (u8*)&(sport1TxBuf.dacs[0].dat), 4 );
}
*/

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
