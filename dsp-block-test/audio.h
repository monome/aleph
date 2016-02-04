/*! audio.h

  static audio configuration class.
  includes I/O and processing buffers and buffer parameters.

 */

#ifndef _BFIN_AUDIO_H_
#define _BFIN_AUDIO_H_

#include "types.h"

/** if this flag is set, attempt to use DMA descriptors
 * to switch buffers and perform deinterleaving.
 * otherwise, copy and deinterleave in the ISR
 */
#define DMA_DEINTERLEAVE_PINGPONG 1

//! channel count
#define CHANNELS 4
//! block size
#define BLOCKSIZE 16
//! bytes per sample
#define SAMPLESIZE 4 // sizeof(fract32)

//! I/O flags
extern volatile u8 audioRxDone;
extern volatile u8 audioTxDone;

//! type for de-interleaved audio buffers
typedef fract32 buffer_t[CHANNELS][BLOCKSIZE];

#if DMA_DEINTERLEAVE_PINGPONG

//! current pointers to processing buffers
buffer_t *audioIn;
buffer_t *audioOut;

//! I/O buffers
__attribute__((l1_data_A))
__attribute__((aligned(32)))
extern buffer_t inputChannels0;

__attribute__((l1_data_B))
__attribute__((aligned(32)))
extern buffer_t inputChannels1;

__attribute__((l1_data_A))
__attribute__((aligned(32)))
extern buffer_t outputChannels0;

__attribute__((l1_data_B))
__attribute__((aligned(32)))
extern buffer_t outputChannels1;

#else
//! I/O buffers
__attribute__((l1_data_A))
__attribute__((aligned(32)))
extern fract32 audioRxBuf[BLOCKSIZE*CHANNELS];

__attribute__((l1_data_B))
__attribute__((aligned(32)))
extern fract32 audioTxBuf[BLOCKSIZE*CHANNELS];

//! separate process buffers
__attribute__((aligned(32)))
extern buffer_t audioIn;

__attribute__((aligned(32)))
extern buffer_t audioOut;

#endif // DMA_DEINTERLEAVE_PINGPONG

#endif // header guard 
