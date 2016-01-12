#ifndef _ALEPH_DSP_PAN_H_
#define _ALEPH_DSP_PAN_H_

#include "types.h"
#include "fract_math.h"

//--------------------------------
// --- simple linear pan
// return panned sum of two inputs
// given pan coefficient in [0,1]
static inline fract32 pan_lin_mix(fract32 inX, fract32 inY, fract32 pan) {
  fract32 ymul = pan;
  fract32 xmul = sub_fr1x32(FR32_MAX, ymul);
  return add_fr1x32( mult_fr1x32x32(inX, xmul), mult_fr1x32x32(inY, ymul) ); 
}

// return coefficients only
// given pan coefficient in [0,1]
static inline void pan_lin_coeff(fract32* a, fract32* b, fract32 pan) {
    // simple linear pan
    *a = pan;
    *b = sub_fr1x32(FR32_MAX, pan);
}

//-----------------
//--- balanced, equal-power

// return panned sum of two inputs 
// given pan coefficient in [0,1]
fract32 pan_bal_mix(fract32 inX, fract32 inY, fract32 pan);

// return just the pan coefficients 
// given pan coefficient in [0,1]
void pan_bal_coeff(fract32* a, fract32* b, fract32 pan);

#endif
