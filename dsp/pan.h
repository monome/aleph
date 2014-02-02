#ifndef _ALEPH_DSP_PAN_H_
#define _ALEPH_DSP_PAN_H_

// return balanced sum of two inputs 
// given pan coefficient in [0,1]
inline fract32 pan_mix(fract32 inX, fract32 inY, fract32 pan);

// return just the balanced coefficients 
// given pan coefficient in [0,1]
inline void pan_coeff(fract32* a, fract32* b, fract32 pan);

#endif
