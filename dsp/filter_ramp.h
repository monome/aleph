/* ramp.h

   dumb linear ramp utilities

*/


#ifndef _ALEPH_DSP_RAMP_H_
#define _ALEPH_DSP_RAMP_H_


#include "types.h"

//---------------------------
//-- fract32 ramp with arbitrary rate and target

typedef struct _filter_ramp {
  fract32 inc; // increment ( [0-1]
  fract32 sinc; // increment (signed according to direction)
  fract32 x; // target value
  fract32 y; // filtered value
  u8 sync;   // flag if target and filtered values are equal
  u8 dec;   // flag if we are decreasing (sign bit of increment would do as well)
  // align
  u8 dum;
} filter_ramp;


// initialize
extern void filter_ramp_init(filter_ramp* f, fract32 in);
// set increment directly
void filter_ramp_set_inc(filter_ramp* f, fract32 slew);
// set input value
extern void filter_ramp_in(filter_ramp* f, fract32 val);
// get next filtered value
extern fract32 filter_ramp_next(filter_ramp* f);


//---------------------------
//-- simpler toggle-ramp class for simple simplicity.

typedef struct _filter_ramp_tog {
  fract32 inc; // increment ( [0-1]
  fract32 sinc; // signed (or zeroed) increment
  fract32 x; // target value
  fract32 y; // filtered value
  u8 sync;   // flag if done ramping
  // align
  u8 dum[2];
} filter_ramp_tog;


// initialize
extern void filter_ramp_tog_init(filter_ramp_tog* f, fract32 in);
// set increment directly
void filter_ramp_tog_set_inc(filter_ramp_tog* f, fract32 slew);
// set input value
extern void filter_ramp_tog_in(filter_ramp_tog* f, fract32 val);
// get next filtered value
extern fract32 filter_ramp_tog_next(filter_ramp_tog* f);



#endif
