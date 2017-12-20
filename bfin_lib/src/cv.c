/// reverting


#include "bfin_core.h"
#include "cv.h"
#include "gpio.h"

// AD5686R 16-bit quad DAC
#define CV_DAC_COM_WRITE 0x3
#define CV_DAC_COM_LSHIFT 20
#define CV_DAC_ADDR_LSHIFT 16

//====================
//=== global variables , initialized here
volatile u32 cvTxBuf = 0x00000000;

//=============================
// extern functions

// intialize the cv dac
void init_cv(void) {
  u32 delay;
  // bring the DAC out of reset
  CV_DAC_RESET_LO;
  delay = 100000;
  while(delay > 0) { delay--; }
  CV_DAC_RESET_HI;
}


// update via DMA
void cv_update(u8 ch, fract32 val) {
  u32 buf;
  buf = 0;
  buf |= (CV_DAC_COM_WRITE << CV_DAC_COM_LSHIFT);
  buf |= ((1 << ch) << CV_DAC_ADDR_LSHIFT);
  // shift from fract32
  buf |= (val >> 15 ) & 0xffff;
  // extra bit for weird FS timing kludge (need 25 clocks)
  cvTxBuf = buf << 1;
}
