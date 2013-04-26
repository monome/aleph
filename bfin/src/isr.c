#include "bfin_core.h"
#include "init.h"
#include "module.h"
#include "protocol.h"
#include "spi.h"
#include "types.h"

#include "isr.h"

//--------- global variables (initialized here)
// 4 channels of input from ad1836
fract32 in0, in1, in2, in3;
// 4 channels of output to ad1836
fract32 out0, out1, out2, out3;

// sport0 receive interrupt (audio input from codec)
void sport0_rx_isr() {
  /// debug:
  //  static u8 leds = 0;
  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer 
  // shift left from 24-bit
  in0 = ( iRxBuf[INTERNAL_ADC_L0] << 8 ) & 0xffffff00;
  in1 = ( iRxBuf[INTERNAL_ADC_R0] << 8 ) & 0xffffff00;
  in2 = ( iRxBuf[INTERNAL_ADC_L1] << 8 ) & 0xffffff00;
  in3 = ( iRxBuf[INTERNAL_ADC_R1] << 8 ) & 0xffffff00;

  /* in0 = iRxBuf[INTERNAL_ADC_L0] ; */
  /* in1 = iRxBuf[INTERNAL_ADC_R0] ; */
  /* in2 = iRxBuf[INTERNAL_ADC_L1] ; */
  /* in3 = iRxBuf[INTERNAL_ADC_R1] ; */
  
  // copy last frame's processing result to DMA output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = out0 >> 8;
  iTxBuf[INTERNAL_DAC_R0] = out1 >> 8;
  iTxBuf[INTERNAL_DAC_L1] = out2 >> 8;
  iTxBuf[INTERNAL_DAC_R1] = out3 >> 8;

  // module-defined frame processing function
  
  module_process_frame();  

  /* //// TEST: wire */
  /* iTxBuf[0] = iRxBuf[0]; */
  /* iTxBuf[1] = iRxBuf[1]; */
  /* iTxBuf[2] = iRxBuf[2]; */
  /* iTxBuf[3] = iRxBuf[3]; */


// module-defined LED update function
  // leds = module_update_leds();
  ///// FIXME
  //  *pFlashA_PortB_Data = leds;
}

// spi receive interrupt (from avr32)
void spi_rx_isr() {
  *pSPI_TDBR = spi_process(*pSPI_RDBR);
}

// programmable flag interrupt (buttons)
/*
void pf_isr() {
  /// debug:
  static u16 butstate = 0;
  /// flag data bits are 1 regardless of edge direction,
  //// so we need to toggle... 
  butstate ^= *pFIO_FLAG_D;
  // module-defined button handler
  //  module_handle_button(*pFIO_FLAG_D);
  module_handle_button(butstate);
  // confirm interrupt handling (W1C)
  *pFIO_FLAG_C = 0x0f00;
}
*/
