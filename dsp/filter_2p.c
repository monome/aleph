/* filter_2p.c
 * null
 * aleph

 two-pole filters

 */

// std
#include <math.h>
// aleph
#include "filter_2p.h"
#include "module.h"

#ifdef ARCH_BFIN
#include "fract_math.h"
#include <fract2float_conv.h>
#else
#include "fract32_emu.h"
#endif

// class-wide temp variable, compute at init
// inverse of samplerate * 2pi
static float rho;

//=========================
//==== external functions

//----------------------
//-------- lowpass

/* TODO
// intialize at pre-allocated memory
void filter_2p_lo_init(filter_2p_lo* f, fract32 in) {
  f->y = in;
  f->x = in;
  f->sync = 1;
  fSrInv = 1.f / (float)SAMPLERATE;
  f->c = FR32_MAX;
}

// set cutoff frequency in hz
void filter_2p_lo_set_hz(filter_2p_lo* f, fix16 hz) {
  f32 fc =  (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) * fSrInv ); // / (float)(f->sr) );
  //  printf("\r1p slewicient: %f\n", fc);
  f->c = float_to_fr32(fc);
}

// set integrator slewicient directly
void filter_2p_lo_set_slew(filter_2p_lo* f, fract32 slew) {
  f->c = slew;
}

// set target value 
void filter_2p_lo_in(filter_2p_lo* f, fract32 val) {
  f->x = val;
  f->sync = (val == f->y);
}

// get next filtered value
fract32 filter_2p_lo_next(filter_2p_lo* f) {
  
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
*/


//---------------------
//---- hipass

// intialize at pre-allocated memory
void filter_2p_hi_init(filter_2p_hi* f) {
  f->y = 0;
  f->x = 0;
  rho = (1.0 / (double)SAMPLERATE) * M_TWOPI;
  filter_2p_hi_calc_coeffs(10.f, 1.4142135623730951f, &(f->a), &(f->b), &(f->g));
}

// get next filtered value
fract32 filter_2p_hi_next(filter_2p_hi* f, fract32 x) {
  /*
  y = 2 * (a * (x + x2 - 2 * x1) + g * y1 - b * y2);
  x2 = x1;
  x1 = x;
  y2 = y1;
  y1 = y;
  */
  f->y = shl_fr1x32(
	      sub_fr1x32(
			 add_fr1x32(
				    mult_fr1x32x32( f->a,
						   add_fr1x32( x, 
							       sub_fr1x32( f->x2,
									   shl_fr1x32( f->x1, 1)
									   )
							       )
						    ),
				    mult_fr1x32x32( f->g, f->y1)
				    ),
			 mult_fr1x32x32( f->b, f->y2)
			 ), 
	      1 /*shift*/);
  f->x2 = f->x1;
  f->x1 = x;
  f->y2 = f->y1;
  f->y1 = f->y;
  return f->y;
}


// set coefficients directly
void filter_2p_hi_set_alpha(filter_2p_hi* f, fract32 a) {
  f->a = a;
}
void filter_2p_hi_set_beta(filter_2p_hi* f, fract32 b) {
  f->b = b; 
}
void filter_2p_hi_set_gamma(filter_2p_hi* f, fract32 g) {
  f->g = g;
}

// calculate coefficients given cutoff in hz and damping factor
void filter_2p_hi_calc_coeffs(float hz, float d, fract32* a, fract32* b, fract32* g  ) {
  const float theta = hz * rho;
  const float tmp = 0.5f * d * sinf(theta);
  *b = float_to_fr32(0.5 * ((1.f - tmp) / (1.f + tmp)) );
  *g = (*b + 0.5f) * cosf(theta);
  *a = (0.5f + *b + *g  )* 0.25f;
}
