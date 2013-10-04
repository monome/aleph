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

  if(!processAudio) { return; }

  // tick the control rate
  //// ... doing this in main() now
  //  ctl_next_frame();
  BUSY_HI;

  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer 
  // shift left from 24-bit
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

  BUSY_LO;
}

// ISR on sport1 tx completion
void sport1_tx_isr() {
  LED4_LO;
  //  u32 stat;
  // DMA_DONE bit is W1C
  // clear interrupt flag, leave enabled
  *pDMA4_IRQ_STATUS = 0x0001;
  //  stat = *pSPORT1_STAT ;
}


// spi receive interrupt (from avr32)
void spi_rx_isr() {
  BUSY_HI;
  // int i=0;
  // processAudio = 0;
  // disable global interrupts
  // asm volatile("cli %0; csync;":"+d"(i));
  *pSPI_TDBR = spi_process(*pSPI_RDBR);
  // enable global interrupts
  // asm volatile("sti %0; csync;":"+d"(i));
  // processAudio = 1;
  BUSY_LO;
}
