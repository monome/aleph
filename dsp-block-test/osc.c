#include <fract.h>
#include <fract_typedef.h>

#include "audio.h"
#include "osc.h"

static const fract32 sine_table[WAVE_TAB_SIZE] = {
#include "sine_table_inc.c"
};

static u32 phase = 0;
static u32 phi = 0;
static fract32 amp = 0x3fffff;

void osc_set_phi(u32 _phi) {
  phi = _phi;
}

void osc_set_amp(fract32 _amp) {
  amp = _amp;
}

void osc_process_block(fract32* dst) {
  u16 i;
  fract32 val;

  u32 idxA;
  u32 idxB;
  fract32 mulA;
  fract32 mulB;

  fract32* p = dst;
  
  for(i=0; i<BLOCKSIZE; i++) {
    // phase is unsigned 32b
    // allow overflow
    phase = phase + phi;
    
    // lookup index
    // shift left for 10-bit index
    idxA = phase >> 22;
    idxB = (idxA + 1) & 1023;

    // use bottom 22 bits for interpolation
    // shift back to frac32 range
    mulB = (fract32) ((phase & ((1<<22)-1)) << 9);
    mulA = sub_fr1x32(0x7fffffff, mulB);

    // lookup, scale, and attenuate
    /* val = mult_fr1x32( */
    /* 		      amp, */
    /* 		      add_fr1x32( */
    /* 				 mult_fr1x32(sine_table[idxA], mulA), */
    /* 				 mult_fr1x32(sine_table[idxB], mulB) */
    /* 				 ) */
    /* 		      ); */

    
    // try just lookup...
    //    val = mult_fr1x32(amp, sine_table[idxA]);
    // ok... just try direct phase!
    ///... wtf
    val = sub_fr1x32((fract32)(phase >> 1),  0x7fffff);
    val = mult_fr1x32(amp, val);
    
    // mix to output buffer (all channels)
    *p = add_fr1x32(*p, val);
    p++;
    *p = add_fr1x32(*p, val);
    p++;
    *p = add_fr1x32(*p, val);
    p++;
    *p = add_fr1x32(*p, val);
    p++;
  }
}


