#include "module.h"
#include "osc.h"

/* // TEST: sawtooth */
/* static volatile fract32 sawVal = 0; */
/* static const fract32 sawInc = 0x02aaaaaa; // should give ~500 hz i think? */


void module_init(void) {
  osc_set_phi(0x00962fc9); // ~220 hz or something
  osc_set_amp(0x3fffffff);
}

void module_process_block(void) {
  /* int i; */
  /* for(i=0; i<BLOCKSIZE; i++) { */
  /*   fract32 val = sawVal >> 8; */
  /*   audioProcessOutBuf[i*4] = val; */
  /*   audioProcessOutBuf[i*4 + 1] = val; */
  /*   audioProcessOutBuf[i*4 + 2] = val; */
  /*   audioProcessOutBuf[i*4 + 3] = val; */
  /*   sawVal += sawInc; */
  /* } */
  
  u16 i;
  fract32* src = audioProcessInBuf;
  fract32* dst = audioProcessOutBuf;
  
  for(i=0; i<BLOCKSIZE; i++) {
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
  }
  
  osc_process_block(audioProcessOutBuf);
}
