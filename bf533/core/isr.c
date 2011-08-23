#include "bf533_audio_core.h"
#include "params.h"
#include "../../common/protocol.h"

// ringbuffer for incoming spi data
unsigned short int spiRxRing[P_PARAM_MSG_WORD_COUNT];
// counter for inndexing spi ringbufer
unsigned short int spiRxRingIdx = 0;
// flag indicating spi rx data needs handling
unsigned char spiRxFlag = 0;

// flag to signal audio frame processing
volatile unsigned char needAudioFrame = 0;

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
  
  // if we have new spi data, deal with it
  if (spiRxFlag) { handle_spi_rx(); }

  // call the module-defined process function on this frame
  //  process_frame();
  
  // copy processed data from variables into dma output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = iOut00 >> 8;
  iTxBuf[INTERNAL_DAC_R0] = iOut01 >> 8;
  iTxBuf[INTERNAL_DAC_L1] = iOut10 >> 8;
  iTxBuf[INTERNAL_DAC_R1] = iOut11 >> 8;

  // tell main loop we need audio
  needAudioFrame = 1;

}

// DEBUG: capure a bunch of spi rx's
// static unsigned short int debugSpiRx[300];
// static unsigned int debugSpiRxCount=0;

// spi receive interrupt (control change from avr32)
void spi_rx_isr() {
  // copy rx data to ringbuffer and get the fuck out of here
  // reading from the rx data register also clears the rx interrupt
  spiRxRingIdx = (spiRxRingIdx + 1) % P_PARAM_MSG_WORD_COUNT;
  spiRxRing[spiRxRingIdx] = *pSPI_RDBR;
  spiRxFlag = 1;

}
