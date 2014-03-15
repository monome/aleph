#include "bfin_core.h"
#include "dac.h"
#include "gpio.h"

// AD5686R 16-bit quad DAC

static const u32 dacWrite = 0x300000;
static const dacChanAddr[4] = { 0x10000, 0x20000, 0x40000, 0x80000 };

//====================
//=== global variables , initialized here
volatile u32 cvTxBuf[2] = { 0x00000000 , 0x00000000 };

//=============================
// extern functions

// intialize the dac
void init_dac(void) {
  u32 delay;
  /// hold LDAC low always
  DAC_LDAC_LO;  

  DAC_RESET_LO;

  // wait
  delay = 100000;
  while(delay > 0) { delay--; }

  DAC_RESET_HI;
}


// update via DMA
void dac_update(int ch, u16 val) {
  const u32 buf = dacWrite | dacChanAddr[ch] | (u32)val;
  cvTxBuf[0] = 0x00000000;
  cvTxBuf[1] = buf;
}
