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
}


// update via DMA
void cv_update(u8 ch, fract32 val) {
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
