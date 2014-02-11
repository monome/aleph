#ifndef _ALEPH_DSP_PAN_H_
#define _ALEPH_DSP_PAN_H_

#include "types.h"

//--------------------------------
// --- simple linear pan
// return panned sum of two inputs
// given pan coefficient in [0,1]
inline fract32 pan_lin_mix(fract32 inX, fract32 inY, fract32 pan);

// return coefficients only
// given pan coefficient in [0,1]
inline void pan_lin_coeff(fract32* a, fract32* b, fract32 pan);


//-----------------
//--- balanced, equal-power

// return panned sum of two inputs 
// given pan coefficient in [0,1]
inline fract32 pan_lin_mix(fract32 inX, fract32 inY, fract32 pan);

// return just the pan coefficients 
// given pan coefficient in [0,1]
inline void pan_lin_coeff(fract32* a, fract32* b, fract32 pan);

#endif
