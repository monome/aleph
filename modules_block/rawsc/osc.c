#include <fract.h>
#include <fract_typedef.h>

#include "audio.h"
#include "osc.h"

const fract32 sine_table[1024] = {
#include "sine_table_inc.c"
};

static u32 phase[NUM_OSCS];
static u32 phi[NUM_OSCS];
static fract32 amp[NUM_OSCS];

void osc_set_phase(u16 idx, u32 val) {
  phase[idx] = val;
}

void osc_set_phi(u16 idx, u32 val) {
  phi[idx] = val;
}

void osc_set_phi_upper(u16 idx, fract32 val) {
  u32 phi_ = (phi[idx] & 0x0000ffff) | (val & 0x7fff0000);
  phi[idx] = phi_;
}

void osc_set_phi_lower(u16 idx, fract32 val) {
  u32 phi_ = (phi[idx] & 0x7fff0000) | ((val >> 15) & 0x0000ffff);
  phi[idx] = phi_;
}

void osc_set_amp(u16 idx, fract32 val) {
  amp[idx] = val;
}

void osc_process_block(u16 idx, buffer_t *outChannels) { 
  u16 frame, channel;
  fract32 val;

  u32 idxA;
  u32 idxB;
  fract32 mulA;
  fract32 mulB;
  fract32 waveA;
  fract32 waveB;
  
  for(frame=0; frame<MODULE_BLOCKSIZE; frame++) {

    // phase is unsigned 32b
    // allow overflow
    phase[idx] += phi[idx];
    
    // lookup index
    // shift left for 10-bit index
    idxA = phase[idx] >> 22;
    idxB = (idxA + 1) & 1023;

    // use bottom 22 bits for interpolation
    // shift back to [0, 7fffffff]
    mulB = (fract32) ((phase[idx] & 0x3fffff) << 9);
    mulA = sub_fr1x32(0x7fffffff, mulB);

    waveA = shr_fr1x32(sine_table[idxA], 1);
    waveB = shr_fr1x32(sine_table[idxB], 1);

    // lookup, scale, and attenuate
    val = mult_fr1x32x32(
    		      amp[idx],
    		      add_fr1x32(
    				 mult_fr1x32x32(waveA, mulA),
    				 mult_fr1x32x32(waveB, mulB)
    				 )
    		      );
    // shift down to 24b
    val >>= 8;

    // mix to output buffer (all channels)
    for(channel=0; channel<AUDIO_CHANNELS; channel++) { 
      (*outChannels)[channel][frame] = add_fr1x32((*outChannels)[channel][frame], val);
    }
  }
}
