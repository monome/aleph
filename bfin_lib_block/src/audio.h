/*! audio.h

  static audio configuration class.
  includes I/O and processing buffers and buffer parameters.

 */

#ifndef _BFIN_AUDIO_H_
#define _BFIN_AUDIO_H_

#include "types.h"

//! block size
#define BLOCKSIZE 8

//! positions in audio frame
#define AUDIO_FRAME_L0 0
#define AUDIO_FRAME_L1 1
#define AUDIO_FRAME_R0 2
#define AUDIO_FRAME_R1 3

//! enable-audio flag
extern volatile u8 processAudio;

//! I/O flags
extern volatile u8 audioRxDone;
extern volatile u8 audioTxDone;

//! I/O buffers
__attribute__((l1_data_A))
__attribute__((aligned(32)))
extern fract32 audioRxBuf[BLOCKSIZE*4];

__attribute__((l1_data_B))
__attribute__((aligned(32)))
extern fract32 audioTxBuf[BLOCKSIZE*4];

//! process buffers
__attribute__((aligned(32)))
extern fract32 audioProcessInBuf[BLOCKSIZE*4];

__attribute__((aligned(32)))
extern fract32 audioProcessOutBuf[BLOCKSIZE*4];

#endif
