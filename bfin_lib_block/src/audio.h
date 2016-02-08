/*! audio.h

  static audio configuration class.
  includes I/O and processing buffers and buffer parameters.

 */

#ifndef _BFIN_AUDIO_H_
#define _BFIN_AUDIO_H_

#include "module_custom.h"
#include "types.h"

//! channel count
#define AUDIO_CHANNELS 4
//! bytes per sample
#define AUDIO_SAMPLESIZE 4 // sizeof(fract32)

//! I/O flags
extern volatile u8 audioRxDone;
extern volatile u8 audioTxDone;

//! audio process flag
extern volatile u8 processAudio;

//! type for de-interleaved audio buffers
typedef fract32 buffer_t[AUDIO_CHANNELS][MODULE_BLOCKSIZE];

//! current pointers to processing buffers
buffer_t *audioIn;
buffer_t *audioOut;

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
