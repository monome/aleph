#include "bfin_core.h"
#include "gpio.h"
#include "init.h"
#include "isr.h"
//#include "leds.h"
#include "types.h"
#include "util.h"

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

#define DAC_MAX 0x7fff

//====================
//=== global variables , initialized here
volatile u32 txBuf = 0;
// which channel to update on next TX interrupt
u8 upCh = 0;

//===============================
//==== static variables
static const u32 chanIncMul = 1;
static const u32 incOff = 1;

static u32 dacData[4] = { 0, 0, 0, 0 };

//static u16 dacData[4] = { 0xcccc, 0xdddd, 0xeeee, 0xffff };
//static u32 txBuf = 0;

//static u8 ledFlag = 0;
// needs update on next TX interrupt
//static u8 updateFlag = 0;
// flag to send setup command
//static u8 dacSetupFlag = 1;
// setup command data
// static const u32 dacSetupCmd = 0x8000;

//======================================
//==== external functions

void update_channel(int ch) {
  static u32 buf;
  dacData[ch] += (ch * chanIncMul + incOff);
  while(dacData[ch] > DAC_MAX) {
    dacData[ch] -= DAC_MAX;
  }
  if(ch == 0) {
    if(dacData[ch] > 0x8000) {
      LED3_SET;
    } else {
      LED3_UNSET;
    }
  }  if(ch == 1) {
    if(dacData[ch] > 0x8000) {
      LED4_SET;
    } else {
      LED4_UNSET;
    }
  }

  buf = 0;
  buf |= (DAC_COM_WRITE << DAC_COM_LSHIFT);
  buf |= ((1 << ch) << DAC_ADDR_LSHIFT);
#if 0
  buf |= dacData[ch];
  txBuf = buf << 1;
#else
  buf |= dacData[ch] & 0xffff;
  txBuf = buf << 1;
  //  txBuf = buf;
#endif
  /// no DMA:
  //  sport1_tx(&txBuf);
}


//===============================
//===== static funcitons
/* static void sport1_tx(u32* data) { */
/*   // wait till buffer becomes empty */
/*    while((*pSPORT1_STAT & TXHRE) == 0) {;;}   */
/*   // copy to tx buffer */
/*   // each write adds 2x16 bits to output FIFO, two writes to fill both channels */
/*   // FIXME: need lshift cause of 25/24bit discrepancy */
/*     *pSPORT1_TX32 = (*data) << 1; */
/*     *pSPORT1_TX32 = (*data) << 1; */
/*   //    *pSPORT1_TX32 = (*data); */
/*   //    *pSPORT1_TX32 = (*data); */
/* } */

/* static void update_dummy(void) { */
/*   //  static const u32 dummy = 0; */
/*   //  sport1_tx(&dummy); */
/*   txBuf = 0; */
/* } */

void timer0_isr() {
  // handle
  *pTIMER_STATUS = 0x0001;
  // toggle LED
  /* if(ledFlag) { */
  /*   *pFIO_FLAG_D &= 0xbfff; */
  /*   ledFlag = 0; */
  /* } else { */
  /*   *pFIO_FLAG_D |= 0x4000; */
  /*   ledFlag = 1; */
  /* } */

  update_channel(upCh);
  upCh++;
  if(upCh == 4) {
    upCh = 0;
  }

  // dac
  //  update_channel(0);
  //  update_channel(1);
  //  update_channel(2);
  //  update_channel(3);
  //  update_dummy();
  
    //updateFlag = 1;
}

void sport1_tx_isr() {
  //  u32 stat;
  //  *pDMA4_IRQ_STATUS = 0x0001;
  //  stat = *pSPORT1_STAT ;
}

//// DEBUG: error handler

void error_isr() {
  u8 dummy = 0;
  u32 sicStat;
  u32 irqStat;
  sicStat = *pSIC_ISR;
  irqStat = *pDMA4_IRQ_STATUS;
  dummy++;
  dummy++;
}
