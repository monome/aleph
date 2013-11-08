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
//#define FR32_COMP_THRESH_NEG 0xfffffffe

// global temp variable, compute at init
// inverse of samplerate as float
static float fSrInv;

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
  f->sync = 1;
  fSrInv = 1.f / (float)SAMPLERATE;
  f->c = 0;
}

// set cutoff frequency in hz
//// warning: this is very slow!
void filter_1p_lo_set_hz(filter_1p_lo* f, fix16 hz) {
  f32 fc =  (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) * fSrInv ); // / (float)(f->sr) );
  //  printf("\r1p slewicient: %f\n", fc);
  f->c = float_to_fr32(fc);
}

// set integrator coefficient directly
void filter_1p_lo_set_slew(filter_1p_lo* f, fract32 slew) {
  f->c = slew;
}

// set target value 
void filter_1p_lo_in(filter_1p_lo* f, fract32 val) {
  f->x = val;
  f->sync = (val == f->y);
}
 
// get next filtered value
fract32 filter_1p_lo_next(filter_1p_lo* f) {
  
  if(f->sync) {
    ;;
  } else {
    f->y = add_fr1x32( f->x,
		       mult_fr1x32x32(f->c,
				      sub_fr1x32(f->y, f->x)
				      ));
    if(fr32_compare(f->x, f->y)) {
      f->y = f->x;
      f->sync = 1;
    }
  }
  return f->y;
}



