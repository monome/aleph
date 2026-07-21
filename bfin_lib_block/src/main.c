// bfin toolchain
#include <blackfin.h>
#include <cdefBF533.h>
#include "ccblkfn.h"

#include "audio.h"
#include "audio_convert.h"
#include "clock_ebiu.h"
#include "cycle_count_aleph.h"
#include "dma.h"
#include "gpio.h"
#include "isr.h"
#include "meters.h"
#include "module.h"
#include "serial.h"
#include "types.h"

#ifndef MODULE_BLOCKSIZE
#error "MODULE_BLOCKSIZE not defined!"
#endif


int main(void) {
  init_clock();
  init_flags();

  READY_LO;

  init_ebiu();

  init_spi_slave();

  init_sport0();
  init_interrupts();
  init_dma();

  module_init();
  meters_init();
  //  isModuleInit = 1;

  enable_dma_sport0();

  // reset the codec
  init_codec();

  LED3_HI;
  LED4_HI;

  READY_HI;

  while(1) {

    if(audioTxDone && audioRxDone && processAudio) {
#if MODULE_AUDIO_XRUN_DETECT
      audioProcIn = audioIn;
      audioProcOut = audioOut;
      audioProcBusy = 1;
      ssync();
#endif
#if MODULE_AUDIO_CONVERT_24_32
      audio_convert_rx_24_to_32(audioIn);
#endif
      module_process_block(audioIn, audioOut);
#if MODULE_AUDIO_METER
      meters_process(audioIn, audioOut);
#endif
#if MODULE_AUDIO_CONVERT_24_32
      audio_convert_tx_32_to_24(audioOut);
#endif
#if MODULE_AUDIO_XRUN_DETECT
      audioProcBusy = 0;
      ssync();
#endif
      audioTxDone = 0;
      audioRxDone = 0;
    }
  }

  return 0;
}
