/* filters.c
 * null
 * aleph
 */

// std
#include <math.h>
// aleph
#include "filters.h"
#include "fract32_emu.h"

#define FR32_COMP_THRESH 0x7

//-----------------------
//---- static functions
static inline u8 fr32_compare(fract32 a, fract32 b) {
  return ( abs_fr1x32(sub_fr1x32(a, b)) > FR32_COMP_THRESH );
}

//-----------------------
//---- external functions

// intialize at pre-allocated memory
void filter_1p_init(filter_1p* f) {
  f->c = 0;
  f->y = 0;
  f->x = 0;
  f->sync = 1;
}

// set cutoff frequency in hz
void filter_1p_set_hz(filter_1p* f, fix16 hz) {
  f->c = float_to_fr32( (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) ) ); 
}

// set target value 
void filter_1p_set(filter_1p* f, fract32 val) {
  f->x = val;
  f->sync = (val == f->y);
}

// get next filtered value
fract32 filter_1p_next(filter_1p* f) {
  f->y = add_fr1x32( f->x,
		     mult_fr1x32x32(f->c,
				    sub_fr1x32(f->y, f->x)
				    ));
  if(fr32_compare(f->x, f->y)) {
    f->y = f->x;
    f->sync = 1;
  }
  return f->y;
}

