#include "fix.h"
#include <jack/jack.h>

fract32 clip_to_fr32(long x) {
  if(x <= (long)(fract32)FR32_MAX && x >= (long)(fract32)FR32_MIN)
    return (fract32) x;
  if(x > (fract32)FR32_MAX)
    return (fract32)FR32_MAX;
  else if (x < (fract32)FR32_MIN)
    return (fract32)FR32_MIN;
}

fract32 mult_fr1x32x32(fract32 x, fract32 y) {
  return (fract32) ( (((long) x) * ((long) y))
		     / ((long) FR32_MAX));
}

fract32 sub_fr1x32(fract32 x, fract32 y) {
  return clip_to_fr32((long) x - (long) y);
}

fract32 add_fr1x32(fract32 x, fract32 y) {
  return clip_to_fr32((long) x + (long) y);
}
