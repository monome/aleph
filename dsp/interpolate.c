#include "fract_math.h"
#include "interpolate.h"

fract32 dsp_lerp32(fract32 a, fract32 b, fract32 c) {
  return add_fr1x32( a, 
		     mult_fr1x32x32( c, sub_fr1x32(b, a)) 
		     );   
}
