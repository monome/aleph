#include <fract_math.h>

#include "fix.h"
#include "types.h"

#include "filter_ramp.h"

//--------------------------------
//-------- fr32 ramp
// intialize at pre-allocated memory
void filter_ramp_init(filter_ramp* f, fract32 in) {
  f->y = in;
  f->x = in;
  f->sync = 1;
  //  fSrInv = 1.f / (float)SAMPLERATE;
  f->inc = 1;
}

// set cutoff frequency in hz
//// warning: this is very slow!
/* void filter_ramp_set_hz(filter_ramp* f, fix16 hz) { */
/*   f32 fc =  (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) * fSrInv ); // / (float)(f->sr) ); */
/*   //  printf("\r1p slewicient: %f\n", fc); */
/*   f->c = float_to_fr32(fc); */
/* } */

// set integrator coefficient directly
void filter_ramp_set_inc(filter_ramp* f, fract32 inc) {
  f->inc = abs_fr1x32( inc );
}

// set target value 
void filter_ramp_in(filter_ramp* f, fract32 val) {
  f->x = val;
  f->sync = (val == f->y);

  if(BIT_SIGN_32(sub_fr1x32(f->y, f->x))) {
    f->sinc = negate_fr1x32(f->inc);
    f->dec = 1;
  } else {
    f->sinc = f->inc;
    f->dec = 0;
  }
		 
}
 
// get next filtered value
fract32 filter_ramp_next(filter_ramp* f) {
  /// FIXME: conditionals suck
  if( !(f->sync) ) {
    f->y = add_fr1x32(f->x, f->sinc);  
    if( f->dec ) {
      if (f->y < f->x) {
	f->y = f->x;
      }
    } else {
      if (f->y > f->x) {
	f->y = f->x;
      }
    }
  }
  return f->y;
}

//---------------------------------------------
//-------- toggled ramp

// intialize at pre-allocated memory
void filter_ramp_tog_init(filter_ramp_tog* f, fract32 in) {
  f->y = in;
  f->x = in;
  f->sync = 1;
  f->inc = 1;
}


void filter_ramp_tog_set_inc(filter_ramp_tog* f, fract32 inc) {
  f->inc = abs_fr1x32( inc );
}

// set target value 
void filter_ramp_tog_in(filter_ramp_tog* f, fract32 val) {
  u8 on = (val > 0);
  if(on) {
    f->x = FR32_MAX;
    if(f->y == FR32_MAX) { 
      f->sinc = 0; 
    } else {
      f->sinc = f->inc;
    }
  } else {
    f->x = 0;
    if(f->y == 0) { 
      f->sinc = 0; 
    } else {
      f->sinc = negate_fr1x32( f->inc );
    }
  }
}
 
// get next filtered value
fract32 filter_ramp_tog_next(filter_ramp_tog* f) {
  // gcc intrinsics are saturating  
  f->y = add_fr1x32(f->y, f->sinc);
  if(f->y < 0) { f->y = 0; }   /// fixme: slow
  f->sync = (f->x == f->y);
  f->sinc = f->sync ? 0 : f->sinc;
  return f->y;

  
}



