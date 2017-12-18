#include <fract_math.h>

#include "fix.h"
#include "types.h"

#include "filter_ramp.h"

//--------------------------------
//-------- fr32 ramp
void filter_ramp_init(filter_ramp* f) {
  f->y = 0;
  f->inc = 1 << 16;
}

void filter_ramp_set_inc(filter_ramp* f, fract32 inc) {
  f->inc = abs_fr1x32( inc );
}

void filter_ramp_start(filter_ramp* f) {
  f->y = 0;
  f->sync = 0;
}
 
// get next filtered value
fract32 filter_ramp_next(filter_ramp* f) {
  // gcc intrinsics are saturating  
  if(f->sync) {
    return FR32_MAX;
  }
  /* printf("not synced %d, %d\n", f->y, f->inc); */
  f->y = add_fr1x32(f->y, f->inc);
  if(f->y >= FR32_MAX) {
    f->sync = 1;
    f->y = FR32_MAX;
  }
  return f->y;
}



