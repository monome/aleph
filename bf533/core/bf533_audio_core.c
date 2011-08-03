#include "bf533_audio_core.h"
#include "sysreg.h"
#include "ccblkfn.h"

//--------- global variables (initialized here)
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
int spiStatus;
unsigned short int spiData;

//-------------------------------
// main function
int main(void) {
  // if we were in VDSP++, this would turn on the execution cycle counter.
  // sysreg_write(reg_SYSCFG, 0x32);
  
// intialize the sdram controller
 init_EBIU();
 // intialize the flash controller (which, weirdly, handles gpio)
  init_flash();
  // initialize the codec (spi in master, blast config regs, disable spi)
  init_1836();
  // intialize the sport0 for audio rx/tx
  init_sport0();
  // intialize the DMA to shove that audio data around
  init_DMA();
  // put the spi back in slave mode to receive param changes from avr32
  init_spi_slave();
  // intialize the audio processing unit (assign memory)
  init_module();
  // assign interrupts
  init_interrupts();
  // start the audio engine
  enable_DMA_sport0();  

  while(1) {
    ;;
  }
}
