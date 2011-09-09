#include "bfin_core.h"
#include "types.h"
#include "module.h"
#include "spi.h"
#include "isr.h"

//--------- global variables (initialized here)
// 4 channels of input from ad1836
fract32 in00, in01, in10, in11;
// 4 channels of output to ad1836
fract32 out00, out01, out10, out11;

// sport0 receive interrupt (audio input from codec)
void sport0_rx_isr() {
  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer 
  // shift left from 24-bit
  // FIXME: data from codec is supposed to be left-justified in i2s mode...?
  in00 = iRxBuf[INTERNAL_ADC_L0] << 8;
  in01 = iRxBuf[INTERNAL_ADC_R0] << 8;
  in10 = iRxBuf[INTERNAL_ADC_L1] << 8;
  in11 = iRxBuf[INTERNAL_ADC_R1] << 8;
  
  // copy last frame's processing result to DMA output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = out00 >> 8;
  iTxBuf[INTERNAL_DAC_R0] = out01 >> 8;
  iTxBuf[INTERNAL_DAC_L1] = out10 >> 8;
  iTxBuf[INTERNAL_DAC_R1] = out11 >> 8;

   // call the module-defined process function on this frame
   process_frame();
}

// spi receive interrupt (control change from avr32)
void spi_rx_isr() {
  // copy rx data to ringbuffer
  // reading from the rx data register also clears the rx interrupt
  spiRxRingIdx = (spiRxRingIdx + 1) % P_PARAM_MSG_WORD_COUNT;
  spiRxRing[spiRxRingIdx] = *pSPI_RDBR;
  handle_spi_rx();
}
