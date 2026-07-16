/*! audio_channels.h

  Map mix/app logical channels (L0, R0, L1, R1) onto SPORT/DMA wire rows
  (L0, L1, R0, R1) in buffer_t[wire][frame]. Matches bfin_lib INTERNAL_ADC_* /
  INTERNAL_DAC_* remapping in sport0_rx_isr.
*/

#ifndef _BFIN_AUDIO_CHANNELS_H_
#define _BFIN_AUDIO_CHANNELS_H_

#include "audio.h"

//! Wire / DMA row indices (SPORT arrival order)
#define IN_L0_IDX  0
#define IN_L1_IDX  1
#define IN_R0_IDX  2
#define IN_R1_IDX  3

#define OUT_L0_IDX 0
#define OUT_L1_IDX 1
#define OUT_R0_IDX 2
#define OUT_R1_IDX 3

//! Logical channel → wire row
#define IN_0_IDX IN_L0_IDX
#define IN_1_IDX IN_R0_IDX
#define IN_2_IDX IN_L1_IDX
#define IN_3_IDX IN_R1_IDX

#define OUT_0_IDX OUT_L0_IDX
#define OUT_1_IDX OUT_R0_IDX
#define OUT_2_IDX OUT_L1_IDX
#define OUT_3_IDX OUT_R1_IDX

static const u8 audioInWireFromLogical[AUDIO_CHANNELS] = {
  IN_0_IDX, IN_1_IDX, IN_2_IDX, IN_3_IDX
};

static const u8 audioOutWireFromLogical[AUDIO_CHANNELS] = {
  OUT_0_IDX, OUT_1_IDX, OUT_2_IDX, OUT_3_IDX
};

//! Pointer to MODULE_BLOCKSIZE samples for a logical input channel
static inline fract32 *audio_in_channel(buffer_t *buf, u8 logical) {
  return (*buf)[audioInWireFromLogical[logical]];
}

//! Pointer to MODULE_BLOCKSIZE samples for a logical output channel
static inline fract32 *audio_out_channel(buffer_t *buf, u8 logical) {
  return (*buf)[audioOutWireFromLogical[logical]];
}

#endif  // header guard
