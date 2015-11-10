#include "audio_process.h"

// TEST: sawtooth
static volatile fract32 sawVal = 0;
static const fract32 sawInc = 0x02aaaaaa; // should give ~500 hz i think?

void process_audio_block(void) {
  int i;
  for(i=0; i<BLOCKSIZE; i++) {
    fract32 val = sawVal >> 8;
    audioProcessOutBuf[i*4] = val;
    audioProcessOutBuf[i*4 + 1] = val;
    audioProcessOutBuf[i*4 + 2] = val;
    audioProcessOutBuf[i*4 + 3] = val;
    sawVal += sawInc;
  }
}
