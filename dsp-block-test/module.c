#include "module.h"
#include "osc.h"

void module_init(void) {
  osc_set_phi(0x00962fc9); // ~220 hz or something
  osc_set_amp(0x3fffffff);
}

void module_process_block(buffer_t *inChannels, buffer_t *outChannels,
			  const u8 numChannels, const u8 numFrames) {
  u16 i, j;
  
  /* fract32* src = inC; */
  /* fract32* dst = audioProcessOutBuf; */
  
  /* for(i=0; i<BLOCKSIZE; i++) { */
  /*   for(j=0; j<CHANNELS */
  /* 	  *dst++ = *src++; */
  /* 	} */
  /* } */

  for(i=0; i<numChannels; i++) {
    for(j=0; j<numFrames; j++) {
      (*outChannels)[i][j] = (*inChannels)[i][j];
    }
  }
  
  //osc_process_block(outChannels, numChannels, numFrames);
}
