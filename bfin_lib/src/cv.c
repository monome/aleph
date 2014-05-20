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
  //#if 0
  u32 delay;
  // bring the DAC out of reset
  //  *pFIO_FLAG_D &= (0xffff ^ (1 << DAC_RESET_PIN));
  CV_DAC_RESET_LO;
  delay = 100000;
  while(delay > 0) { delay--; }
  //  *pFIO_FLAG_D |= (1 << DAC_RESET_PIN);
  CV_DAC_RESET_HI;
  //#endif
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


/* #include "bfin_core.h" */
/* #include "dac.h" */
/* #include "gpio.h" */

/* // AD5686R 16-bit quad DAC */

/* static const u32 dacWrite = 0x300000; */
/* static const u32 dacChanAddr[4] = { 0x10000, 0x20000, 0x40000, 0x80000 }; */

/* //==================== */
/* //=== global variables , initialized here */
/* volatile u32 cvTxBuf[2] = { 0x00000000 , 0x00000000 }; */

/* //============================= */
/* // extern functions */

/* // intialize the dac */
/* void init_dac(void) { */
/*   u32 delay; */
/*   /// hold LDAC low always */
/*   DAC_LDAC_LO;   */

/*   DAC_RESET_LO; */

/*   // wait */
/*   delay = 100000; */
/*   while(delay > 0) { delay--; } */

/*   DAC_RESET_HI; */
/* } */


/* // update via DMA */
/* void dac_update(int ch, u16 val) { */
/*   const u32 buf = dacWrite | dacChanAddr[ch] | (u32)val;  */
/*   cvTxBuf[0] = 0x00000000; */
/*   cvTxBuf[1] = buf; */
/* } */
