#include "fix.h"
#include "pan.h"
#include "table.h"

#define PAN_SINE_BUF_SIZE 1024
#define PAN_SINE_BUF_SIZE_FIX (1024 << 16)

static fract32 sineTab[PAN_SINE_BUF_SIZE] = 
#include "halfsine_1024.inc"
  ;

// return balanced sum of two inputs 
// given pan coefficient in [0,1]
fract32 pan_mix(fract32 inX, fract32 inY, fract32 pan) {
  fix16 xIdx = fix16_mul(FRACT_FIX16(pan), PAN_SINE_BUF_SIZE_FIX);
  fix16 yIdx = fix16_sub(PAN_SINE_BUF_SIZE_FIX, xIdx);
  fract32 xmul = table_lookup_idx(sineTab, PAN_SINE_BUF_SIZE, xIdx);
  fract32 ymul = table_lookup_idx(sineTab, PAN_SINE_BUF_SIZE, yIdx);
  return add_fr1x32( mult_fr1x32x32(inX, xmul), mult_fr1x32x32(inY, ymul) ); 
}

// return just the balanced coefficients 
// given pan coefficient in [0,1]
inline void pan_coeff(fract32* a, fract32* b, fract32 pan) {
  fix16 xIdx = fix16_mul(FRACT_FIX16(pan), PAN_SINE_BUF_SIZE_FIX);
  fix16 yIdx = fix16_sub(PAN_SINE_BUF_SIZE_FIX, xIdx);
  *a = table_lookup_idx(sineTab, PAN_SINE_BUF_SIZE, xIdx);
  *b = table_lookup_idx(sineTab, PAN_SINE_BUF_SIZE, yIdx);
}
