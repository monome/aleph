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

//static u16 dacData[4] = { 0, 0, 0, 0 };
static u16 dacData[4] = { 0xcccc, 0xdddd, 0xeeee, 0xffff };
static u32 txBuf = 0;

static u8 ledFlag = 0;
// needs update on next TX interrupt
static u8 updateFlag = 0;
// which channel to update on next TX interrupt
static u8 upCh = 0;

static void sport1_tx(u32* data) {
  // wait till buffer becomes empty
  //  while((*pSPORT1_STAT & TXHRE) == 0) {;;}  
  // copy to tx buffer
  // each write adds 2x16 bits to output FIFO, two writes to fill both channels
  // FIXME: need lshift cause of 25/24bit discrepancy
  //  *pSPORT1_TX32 = (*data) << 2;
  //  *pSPORT1_TX32 = (*data) << 2;
  //    *pSPORT1_TX32 = (*data);
  //    *pSPORT1_TX32 = (*data);
}

static void update_channel(int ch) {
  dacData[ch] += (ch * chanIncMul + chanIncOff);
  txBuf = 0;
  txBuf |= (DAC_COM_WRITE << DAC_COM_LSHIFT);
  txBuf |= ((1 << ch) << DAC_ADDR_LSHIFT);
  txBuf |= dacData[ch];
  //  sport1_tx(&txBuf);
}

static void update_dummy(void) {
  static const u32 dummy = 0;
  sport1_tx(&dummy);
}

void timer0_isr() {
  // handle
  *pTIMER_STATUS = 0x0001;
  // toggle LED
  if(ledFlag) {
    *pFIO_FLAG_D &= 0xbfff;
    ledFlag = 0;
  } else {
    *pFIO_FLAG_D |= 0x4000;
    ledFlag = 1;
  }
  // dac
  /*
  update_channel(0);
  update_channel(1);
  update_channel(2);
  update_channel(3);
  update_dummy();
  */
  updateFlag = 1;
}

void sport1_tx_isr() {
  u32 stat;
 
  //  if(updateFlag) {
  /// test: 
  //if(1) {
  stat = *pSPORT1_STAT ;
  if((stat & TXHRE) || (stat & TUVF)) {
    update_channel(upCh);
    *pSPORT1_TX32 = txBuf << 1;
    *pSPORT1_TX32 = txBuf << 1;
    upCh++;
    if(upCh == 4) {
      upCh = 0;
      updateFlag = 0;
      *pSPORT1_TX32 = 0;
      *pSPORT1_TX32 = 0;
    }
  }
  
  //  dum++;
  
  //  *pDMA4_IRQ_STATUS = 0x0001;
  
}


//// DEBUG: error handler
/*
void error_isr() {
  u8 dummy = 0;
  u32 irqStatus;
  irqStatus = *pSIC_ISR;
  dummy++;
  dummy++;
}
*/
