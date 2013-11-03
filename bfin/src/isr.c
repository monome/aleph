#include "bfin_core.h"
#include "control.h"
#include "gpio.h"
#include "init.h"
#include "module.h"
#include "protocol.h"
#include "spi.h"
#include "types.h"

#include "isr.h"

//--------- global variables (initialized here)
// 4 channels of input from ad1836
//fract32 in0, in1, in2, in3;
fract32 in[4] = { 0, 0, 0, 0 };
// 4 channels of output to ad1836
//fract32 out0, out1, out2, out3;
fract32 out[4] = { 0, 0, 0, 0 };


// audio processing flag
volatile u8 processAudio = 0;

// sport0 receive interrupt (audio input from codec)
void sport0_rx_isr() {
  //  static u32 dma4count = 0;

  // tick the control rate
  //  ctl_next_frame();

  READY_LO;
  // confirm interrupt handling  
  
  // and if it came from DMA4, clear that and return
  //// not enabling DMA4 interrupt...
  /* if(*pDMA4_IRQ_STATUS & 1) {  */
  /*   *pDMA4_IRQ_STATUS = 0x0001;  */
  /*   //    ++dma4count; */
  /*   //    if(dma4count == 10000) { */
  /*         LED4_TOGGLE; */
  /*     //    } */
  /*   //    LED3_SET; */
  /*   return;  */
  /* } */

  if(!processAudio) { 
    READY_HI;
/// if this interrupt came from DMA1, clear it and continue(W1C)
    if(*pDMA1_IRQ_STATUS & 1) { *pDMA1_IRQ_STATUS = 0x0001; }
    return;
  }

  /// inform the world that we're busy processing an audio frame
  //  BUSY_HI;
  //  LED3_UNSET;
  

  // copy input data from dma input buffer 
  // shift left fr21om 24-bit
  in[0] = ( iRxBuf[INTERNAL_ADC_L0] << 8 ) & 0xffffff00;
  in[1] = ( iRxBuf[INTERNAL_ADC_R0] << 8 ) & 0xffffff00;
  in[2] = ( iRxBuf[INTERNAL_ADC_L1] << 8 ) & 0xffffff00;
  in[3] = ( iRxBuf[INTERNAL_ADC_R1] << 8 ) & 0xffffff00;

  // copy last frame's processing result to DMA output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = out[0] >> 8;
  iTxBuf[INTERNAL_DAC_R0] = out[1] >> 8;
  iTxBuf[INTERNAL_DAC_L1] = out[2] >> 8;
  iTxBuf[INTERNAL_DAC_R1] = out[3] >> 8;

  // module-defined frame processing function

  module_process_frame();  

  /* //// TEST: wire */
  /* out0 = in0; */
  /* out1 = in1; */
  /* out2 = in2; */
  /* out3 = in3; */

  /* //// TEST: wire */
  /* iTxBuf[0] = iRxBuf[0]; */
  /* iTxBuf[1] = iRxBuf[1]; */
  /* iTxBuf[2] = iRxBuf[2]; */
  /* iTxBuf[3] = iRxBuf[3]; */

  //  BUSY_LO;


  //  LED3_SET;

  READY_HI;
  /// if this interrupt came from DMA1, clear it and continue(W1C)
  if(*pDMA1_IRQ_STATUS & 1) { *pDMA1_IRQ_STATUS = 0x0001; }

}

// ISR on sport1 tx completion
void sport1_tx_isr() {
  //  u32 stat;
  // clear the interrupt flag, leave enabled
  *pDMA4_IRQ_STATUS = 0x0001;
  //  stat = *pSPORT1_STAT ;
}


// spi receive interrupt (from avr32)
void spi_rx_isr() {
  //  BUSY_HI;
  READY_LO;
  *pSPI_TDBR = spi_process(*pSPI_RDBR);
  READY_HI;
  //  BUSY_LO;
}
