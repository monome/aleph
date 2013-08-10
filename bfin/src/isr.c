#include "bfin_core.h"
#include "control.h"
#include "init.h"
#include "leds.h"
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
  // dbg
  /* static s32 max = 0; */
  /* static s32 min = 0; */
  /* static u8 dum = 0; */

  /* if(in0 > max) { */
  /*   max = in0; */
  /*   dum++; */
  /* } */
  /* if(in0 < min) { */
  /*   min = in0; */
  /*   dum++; */
  /* } */

  /* if(iRxBuf[0] < 0) { */
  /*    dum++; */
  /* } */

  // tick the control rate
  ctl_next_frame();


  UNSET_LED3;

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
  if(processAudio) {
    module_process_frame();  
  }

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

  SET_LED3;
// module-defined LED update function
  // leds = module_update_leds();
  ///// FIXME
  //  *pFlashA_PortB_Data = leds;
}

// spi receive interrupt (from avr32)
void spi_rx_isr() {
  //int i=0;
  //  processAudio = 0;
  // disable global interrupts
  //  asm volatile("cli %0; csync;":"+d"(i));
  *pSPI_TDBR = spi_process(*pSPI_RDBR);
  // enable global interrupts
  //  asm volatile("sti %0; csync;":"+d"(i));
  //  processAudio = 1;
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
