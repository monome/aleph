#include "audio.h"

// globally visible cycle count result
// volatile u64 audioCycleCount;


// I/O flags
volatile u8 audioRxDone = 0;
volatile u8 audioTxDone = 0;

volatile u8 processAudio = 0;


// I/O buffers
buffer_t inputChannels0;
buffer_t inputChannels1;
buffer_t outputChannels0;
buffer_t outputChannels1;

buffer_t* volatile audioIn = &inputChannels0;
buffer_t* volatile audioOut = &outputChannels0;

#if MODULE_AUDIO_XRUN_DETECT
volatile u8 audioProcBusy = 0;
buffer_t* volatile audioProcIn = 0;
buffer_t* volatile audioProcOut = 0;

volatile u16 xrunWindowRx = 0;
volatile u16 xrunWindowTx = 0;
volatile u16 xrunClashRx = 0;
volatile u16 xrunClashTx = 0;
#endif

void audio_reset_xruns(void) {
#if MODULE_AUDIO_XRUN_DETECT
  xrunWindowRx = 0;
  xrunWindowTx = 0;
  xrunClashRx = 0;
  xrunClashTx = 0;
#endif
}
