/* filters.c
 * null
 * aleph
 */

// std
#include <math.h>
// aleph
#include "filter_1p.h"
#include "module.h"

#ifdef ARCH_BFIN
#include "fract_math.h"
#include <fract2float_conv.h>
#else
#include "fract32_emu.h"
#endif

// a little under -120db
#define FR32_COMP_THRESH 0x800

//-----------------------
//---- static functions

static inline u8 fr32_compare(fract32 a, fract32 b) {
  //  fract32 dif = sub_fr1x32(a, b);
  //  return ( ( dif < FR32_COMP_THRESH ) && ( dif > FR32_COMP_THRESH_NEG ) );
  return ( abs_fr1x32(sub_fr1x32(a, b)) < FR32_COMP_THRESH );
}


//-----------------------
//---- external functions


//-------- lowpass
// intialize at pre-allocated memory
void filter_1p_lo_init(filter_1p_lo* f, fract32 in) {
  f->y = in;
  f->x = in;
  f->c = 0;
}

// set cutoff frequency in hz
/// warning: this is very slow!
/// in fact, just don't use it on blackfin if it can be avoided.
/// preferably, send precalculated values directly.
/* void filter_1p_lo_set_hz(filter_1p_lo* f, fix16 hz) { */
/*   f32 fc =  (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) * fSrInv ); // / (float)(f->sr) ); */
/*   //  printf("\r1p slewicient: %f\n", fc); */
/*   f->c = float_to_fr32(fc); */
/* } */

// set integrator coefficient directly
void filter_1p_lo_set_slew(filter_1p_lo* f, fract32 slew) {
  f->c = slew;
}

// set target value 
void filter_1p_lo_in(filter_1p_lo* f, fract32 val) {
  f->x = val;
}
 
// get next filtered value
fract32 filter_1p_lo_next(filter_1p_lo* f) {
  f->y = add_fr1x32( f->x,
		     mult_fr1x32x32(f->c,
				    sub_fr1x32(f->y, f->x)
				    ));
  return f->y;
}



// compare target/value
extern u8 filter_1p_sync(filter_1p_lo* f) {
  // early return if equal
  if(f->x == f->y) { 
    return 1; 
  } 
  else {
    if ( fr32_compare(f->x, f->y) ) {
      f->y = f->x;
      // return 0, but next query will be true
      return 0;
    } else {
      // still converging
      return 0;
    }
  }
}
