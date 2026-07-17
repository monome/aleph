/*! audio.h

  static audio configuration class.
  includes I/O and processing buffers and buffer parameters.

 */

#ifndef _BFIN_AUDIO_H_
#define _BFIN_AUDIO_H_

#include "module_custom.h"
#include "types.h"

#ifndef MODULE_BLOCKSIZE
#define MODULE_BLOCKSIZE 16
#endif

#ifndef MODULE_AUDIO_CONVERT_24_32
#define MODULE_AUDIO_CONVERT_24_32 0
#endif

#ifndef MODULE_AUDIO_XRUN_DETECT
#define MODULE_AUDIO_XRUN_DETECT 0
#endif

//! channel count
#define AUDIO_CHANNELS 4
//! bytes per sample
#define AUDIO_SAMPLESIZE 4 // sizeof(fract32)
//! samples per second
#define AUDIO_SAMPLERATE 48000

//! I/O flags
extern volatile u8 audioRxDone;
extern volatile u8 audioTxDone;

//! audio process flag
extern volatile u8 processAudio;

//! reset block-audio xrun counters (no-op when detection is disabled)
extern void audio_reset_xruns(void);

//! keep track of cycles spent processing each audio block
// extern volatile u64 audioCycleCount;

//! type for de-interleaved audio buffers
typedef fract32 buffer_t[AUDIO_CHANNELS][MODULE_BLOCKSIZE];

//! current pointers to processing buffers
extern buffer_t * volatile audioIn;
extern buffer_t * volatile audioOut;

#if MODULE_AUDIO_XRUN_DETECT
//! main publishes these while converting/processing a block
extern volatile u8 audioProcBusy;
extern buffer_t * volatile audioProcIn;
extern buffer_t * volatile audioProcOut;

//! xrun counters (wrap on overflow)
extern volatile u16 xrunWindowRx;
extern volatile u16 xrunWindowTx;
extern volatile u16 xrunClashRx;
extern volatile u16 xrunClashTx;
#endif

//! I/O buffers
__attribute__((l1_data_A))
__attribute__((aligned(32)))
extern buffer_t inputChannels0;

__attribute__((l1_data_A))
__attribute__((aligned(32)))
extern buffer_t inputChannels1;

__attribute__((l1_data_B))
__attribute__((aligned(32)))
extern buffer_t outputChannels0;

__attribute__((l1_data_B))
__attribute__((aligned(32)))
extern buffer_t outputChannels1;


#endif // header guard
