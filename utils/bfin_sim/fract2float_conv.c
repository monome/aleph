#include "fix.h"
#include "fract2float_conv.h"
#include "types.h"

fract32 fr16_to_fr32(fract16 _x) {
  unsigned int val = ((unsigned int)(_x)) << 16;
  return (fract32)val;
}

fract16 fr32_to_fr16(fract32 _x) {
  unsigned int val = (unsigned int)_x >> 16;
  return (fract16)val;
}

fract32 float_to_fr32 (float in) {
  double ret = (((double) in) * ((double) FR32_MAX));
  if (ret > ((double) ((fract32) FR32_MAX))) {
    return FR32_MAX;
  } else if (ret < ((double) ((fract32) FR32_MIN))) {
    return FR32_MIN;
  } else {
    return (fract32) ret;
  }
}

float fr32_to_float (fract32 in) {
  double ret = ((double) in) / ((double) ((fract32) FR32_MAX));
  return (float) ret;
}


fract16 float_to_fr16 (float _x);

float   fr32_to_float (fract32 _x);
float   fr16_to_float (fract16 _x);


