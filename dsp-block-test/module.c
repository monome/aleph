#include "module.h"
#include "osc.h"

void module_init(void) {
  osc_set_phi(0x00962fc9); // ~220 hz or something
  osc_set_amp(0x3fffffff);
}

void module_process_block(fract32** inChannels, fract32** outChannels, u8 numChannels, u8 numFrames) {
  u16 i, j;
  
  fract32* src = audioProcessInBuf;
  fract32* dst = audioProcessOutBuf;
  
  for(i=0; i<BLOCKSIZE; i++) {
    for(j=0; j<CHANNELS
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
  }
  
  osc_process_block(audioProcessOutBuf);
}
