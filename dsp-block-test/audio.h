/*! audio.h

  static audio configuration class.
  includes I/O and processing buffers and buffer parameters.

 */

#ifndef _BFIN_AUDIO_H_
#define _BFIN_AUDIO_H_

#include "types.h"

//! block size
#define CHANNELS 4
#define BLOCKSIZE 16
#define SAMPLESIZE 4 // sizeof(fract32)

//! positions in audio frame
#define AUDIO_FRAME_L0 0
#define AUDIO_FRAME_L1 1
#define AUDIO_FRAME_R0 2
#define AUDIO_FRAME_R1 3

//! I/O flags
extern volatile u8 audioRxDone;
extern volatile u8 audioTxDone;

//! current pointers to audio buffers
fract32** audioIn;
fract32** audioOut;

//! I/O buffers
__attribute__((l1_data_A))
__attribute__((aligned(32)))
fract32 inputChannels0[CHANNELS][BLOCKSIZE];

__attribute__((l1_data_A))
__attribute__((aligned(32)))
fract32 inputChannels1[CHANNELS][BLOCKSIZE];

__attribute__((l1_data_A))
__attribute__((aligned(32)))
fract32 outputChannels0[CHANNELS][BLOCKSIZE];

__attribute__((l1_data_A))
__attribute__((aligned(32)))
fract32 outputChannels1[CHANNELS][BLOCKSIZE];

/* __attribute__((l1_data_A)) */
/* __attribute__((aligned(32))) */
/* extern fract32 audioRxBuf[BLOCKSIZE*4]; */

/* __attribute__((l1_data_B)) */
/* __attribute__((aligned(32))) */
/* extern fract32 audioTxBuf[BLOCKSIZE*4]; */

/* //! process buffers */
/* __attribute__((aligned(32))) */
/* extern fract32 audioProcessInBuf[BLOCKSIZE*4]; */

/* __attribute__((aligned(32))) */
/* extern fract32 audioProcessOutBuf[BLOCKSIZE*4]; */

#endif
