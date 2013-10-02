/* filter_2p.h
 * null
 * aleph
 *
 * simple 2-pole filters (lowpass and highpass)
 */

#ifndef _ALEPH_AUDIO_FILTER_2POLE_H_
#define _ALEPH_AUDIO_FILTER_2POLE_H_

#include "fix.h"
#include "types.h"

//-- lowpass 
/*
typedef struct _filter_2p_lo {
  //  u32 sr;    // sample rate
  fract32 a; 	// alpha
  fract32 b; 	// beta
  fract32 g; 	// gamma
  fract32 x; 	// input
  fract32 x1; 	// input at (t-1)
  fract32 x2; 	// input at (t-2)
  fract32 y; 	// output
  fract32 y1; 	// output at (t-1)
  fract32 y2; 	// output at (t-2)
} filter_2p_lo;
*/

//-- highpass
typedef struct _filter_2p_hi {
  //  u32 sr;    // sample rate
  fract32 a; 	// alpha
  fract32 b; 	// beta
  fract32 g; 	// gamma
  fract32 x; 	// input
  fract32 x1; 	// input at (t-1)
  fract32 x2; 	// input at (t-2)
  fract32 y; 	// output
  fract32 y1; 	// output at (t-1)
  fract32 y2; 	// output at (t-2)
} filter_2p_hi;


///---- lowpass
// initialize
/* TODO
extern void filter_2p_lo_init(filter_1p_lo* f, fract32 in);
// set cutoff frequency in hz
extern void filter_2p_lo_set_hz(filter_1p_lo* f, fix16 hz);
// set coefficients directly
void filter_2p_lo_set_alpha(filter_1p_lo* f, fract32 a);
void filter_2p_lo_set_beta(filter_1p_lo* f, fract32 b);
void filter_2p_lo_set_gamma(filter_1p_lo* f, fract32 g);
// set input value
extern void filter_2p_lo_in(filter_1p_lo* f, fract32 val);
// get next filtered value
extern fract32 filter_2p_lo_next(filter_1p_lo* f);
*/

///---- highpass
// initialize
extern void filter_2p_hi_init(filter_2p_hi* f);
// set cutoff frequency in hz
//extern void filter_2p_hi_set_hz(filter_2p_hi* f, fix16 hz);
// set coefficients
extern void filter_2p_hi_set_alpha(filter_2p_hi* f, fract32 a);
extern void filter_2p_hi_set_beta(filter_2p_hi* f, fract32 b);
extern void filter_2p_hi_set_gamma(filter_2p_hi* f, fract32 g);
// input and get next output
extern fract32 filter_2p_hi_next(filter_2p_hi* f, fract32 val);

// calculate coefficients 
// hz is cutoff frequency
// d is rolloff, reciprocal of Q:
// d = 1/R * sqrt(L/C) in RCL network
extern void filter_2p_hi_calc_coeffs(float hz, float d, 
					fract32* a, fract32* b, fract32* g);

#endif
