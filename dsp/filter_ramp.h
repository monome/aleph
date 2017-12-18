/* ramp.h

   dumb linear ramp utilities

*/


#ifndef _ALEPH_DSP_RAMP_H_
#define _ALEPH_DSP_RAMP_H_


#include "types.h"

typedef struct _filter_ramp {
  fract32 inc; // increment ( [0-1]
  fract32 y; // filtered value
  u8 sync;   // flag if done ramping
} filter_ramp;


// initialize
extern void filter_ramp_init(filter_ramp* f);
// set increment directly
void filter_ramp_set_inc(filter_ramp* f, fract32 inc);
// set input value
extern void filter_ramp_start(filter_ramp* f);
// get next filtered value
extern fract32 filter_ramp_next(filter_ramp* f);



#endif
