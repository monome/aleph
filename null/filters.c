/* filters.c
 * null
 * aleph
 */

// std
#include <math.h>
// aleph
#include "filters.h"
#include "fract32_emu.h"

//#define FR32_COMP_THRESH 0x7
#define FIX16_COMP_THRESH 0x70000
#define FIX16_COMP_THRESH_NEG 0xfff8ffff

//-----------------------
//---- static functions
/*
static inline u8 fr32_compare(fract32 a, fract32 b) {
  return ( abs_fr1x32(sub_fr1x32(a, b)) > FR32_COMP_THRESH );
}
*/

static inline u8 fix16_compare(fix16 a, fix16 b) {
  //  return ( abs_fr1x32(sub_fr1x32(a, b)) > FIX16_COMP_THRESH );
  fix16 dif = fix16_sub(a, b);
  return ( ( dif < FIX16_COMP_THRESH ) && ( dif > FIX16_COMP_THRESH_NEG ) );
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
  f->c = fix16_from_float( (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) ) ); 
}

// set target value 
void filter_1p_set(filter_1p* f, fix16 val) {
  f->x = val;
  f->sync = (val == f->y);
}

// get next filtered value

fix16 filter_1p_next(filter_1p* f) {
  f->y = fix16_add( f->x,
		     fix16_mul(f->c,
				    fix16_sub(f->y, f->x)
				    ));
  if(fix16_compare(f->x, f->y)) {
    f->y = f->x;
    f->sync = 1;
  }
  return f->y;
}

/*
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

*/
