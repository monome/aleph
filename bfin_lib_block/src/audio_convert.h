/*! audio_convert.h

  In-place 24-bit SPORT ↔ fract32 packing for block audio buffers.
  Mirrors bfin_lib sport0_rx_isr shifts; intended for use around
  module_process_block when MODULE_AUDIO_CONVERT_24_32 is enabled.
*/

#ifndef _BFIN_AUDIO_CONVERT_H_
#define _BFIN_AUDIO_CONVERT_H_

#include "audio.h"

#define AUDIO_CONVERT_NSAMP (AUDIO_CHANNELS * MODULE_BLOCKSIZE)

//! Expand 24-bit DMA words to fract32 (<< 8, clear low byte).
static inline void audio_convert_rx_24_to_32(buffer_t *buf) {
  fract32 *p = &(*buf)[0][0];
  fract32 *const end = p + AUDIO_CONVERT_NSAMP;

  while(p + 1 < end) {
    fract32 a = p[0];
    fract32 b = p[1];
    p[0] = (a << 8) & 0xffffff00;
    p[1] = (b << 8) & 0xffffff00;
    p += 2;
  }
  if(p < end) {
    *p = (*p << 8) & 0xffffff00;
  }
}

//! Pack fract32 samples back to 24-bit DMA words (>> 8).
static inline void audio_convert_tx_32_to_24(buffer_t *buf) {
  fract32 *p = &(*buf)[0][0];
  fract32 *const end = p + AUDIO_CONVERT_NSAMP;

  while(p + 1 < end) {
    fract32 a = p[0];
    fract32 b = p[1];
    p[0] = a >> 8;
    p[1] = b >> 8;
    p += 2;
  }
  if(p < end) {
    *p = *p >> 8;
  }
}

#endif  // header guard
