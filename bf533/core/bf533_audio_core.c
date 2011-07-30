#include "bf533_audio_core.h"
#include "sysreg.h"
#include "ccblkfn.h"

// 4 channels of input from ad1836
int iIn00, iIn01, iIn10, iIn11;
// 4 channels of output to ad1836
int iOut00, iOut01, iOut10, iOut11;

// array of 1836 config registers
volatile short sCodec1836TxRegs[CODEC_1836_REGS_LENGTH] = {
  DAC_CONTROL_1	| 0x000,
  DAC_CONTROL_2	| 0x000,
  DAC_VOLUME_0	| 0x3ff,
  DAC_VOLUME_1	| 0x3ff,
  DAC_VOLUME_2	| 0x3ff,
  DAC_VOLUME_3	| 0x3ff,
  DAC_VOLUME_4	| 0x000,
  DAC_VOLUME_5	| 0x000,
  ADC_CONTROL_1	| 0x000,
  ADC_CONTROL_2	| 0x000,
  ADC_CONTROL_3	| 0x000
};

// SPORT0 DMA transmit buffer
volatile int iTxBuf[4];
// SPORT0 DMA receive buffer
volatile int iRxBuf[4];

/// debug
static int spi_status;
static int i=0;

//-------------------------------
// main function
int main(void) {
  // if we were in VDSP++, this would turn on the execution cycle counter.
  // sysreg_write(reg_SYSCFG, 0x32);
  init_EBIU();
  init_flash();
  /// no //test: // don't config the codec
  // init_1836();
  init_sport0();
  init_DMA();
  init_interrupts();
  init_spi_slave();
  enable_DMA_sport0();
  
  // do everything else from ISRs
  while(1) {
    //debug
    spi_status = *pSPI_STAT;
    i++;
  }
}
