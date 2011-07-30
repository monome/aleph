#include "bf533_audio_core.h"
#include "../../common/foo.h"



// sport0 receive interrupt (audio input from codec)
void sport0_rx_isr() {
  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer into variables
  // shift left from 24-bit
  // FIXME: data from codec is supposed to be left-justified in i2s mode...
  iIn00 = iRxBuf[INTERNAL_ADC_L0] << 8;
  iIn01 = iRxBuf[INTERNAL_ADC_R0] << 8;
  iIn10 = iRxBuf[INTERNAL_ADC_L1] << 8;
  iIn11 = iRxBuf[INTERNAL_ADC_R1] << 8;
  
  // call the module-defined process function
  process_frame();
  
  // copy processed data from variables into dma output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = iOut00 >> 8;
  iTxBuf[INTERNAL_DAC_R0] = iOut01 >> 8;
  iTxBuf[INTERNAL_DAC_L1] = iOut10 >> 8;
  iTxBuf[INTERNAL_DAC_R1] = iOut11 >> 8;
}

// spi receive interrupt (control change from avr32)
void spi_rx_isr() {
  unsigned int spiData; 
  // reading the spi receive data register also clears the interrupt
  spiData = *pSPI_RDBR;

  

  // TEST: set LEDs to last 6 bits of spi data
  *pFlashA_PortB_Data &= (~spiData & 0x3F) ;
  *pFlashA_PortB_Data |= (spiData & 0x3F) ;

  // 
}
