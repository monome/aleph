#include "types.h"
#include "gpio.h"

// initialize programmable flags
void init_flags(void) {
  // outputs
  *pFIO_DIR = 0;
  *pFIO_DIR |= CODEC_RESET_UNMASK;
  *pFIO_DIR |= CODEC_RESET_UNMASK;
  *pFIO_DIR |= CV_DAC_RESET_UNMASK;
  *pFIO_DIR |= CV_DAC_LDAC_UNMASK;
  *pFIO_DIR |= READY_UNMASK;
  *pFIO_DIR |= LED3_UNMASK;
  *pFIO_DIR |= LED4_UNMASK;
}

// configure audio codec ( AD1939 )
void init_codec(void) { 
  volatile u32 del;

  //// reset codec
  *pFIO_FLAG_D &= CODEC_RESET_MASK;
  del = 100; while(del--) { ;; } 
  *pFIO_FLAG_D |= (0xffff ^ CODEC_RESET_MASK);
  del = 10000; while(del--) { ;; } 
  return;

} // init_codec

