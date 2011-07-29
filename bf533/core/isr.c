#include "bf533_audio_core.h"

//EX_INTERRUPT_HANDLER(Sport0_RX_ISR)
void sport0_RX_ISR() {
  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer into variables
  iIn00 = iRxBuf[INTERNAL_ADC_L0];
  iIn01 = iRxBuf[INTERNAL_ADC_R0];
  iIn10 = iRxBuf[INTERNAL_ADC_L1];
  iIn11 = iRxBuf[INTERNAL_ADC_R1];
  
  // call the module-defined process function
  process_frame();
  
  // copy processed data from variables into dma output buffer
  iTxBuf[INTERNAL_DAC_L0] = iOut00;
  iTxBuf[INTERNAL_DAC_R0] = iOut01;
  iTxBuf[INTERNAL_DAC_L1] = iOut10;
  iTxBuf[INTERNAL_DAC_R1] = iOut11;
}


