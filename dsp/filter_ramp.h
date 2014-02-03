/* filter_amp.h

   dumb linear ramp utility

*/


#ifndef _ALEPH_DSP_RAMP_H_
#define _ALEPH_DSP_RAMP_H_


#include "types.h"

//-- lowpass fr32
typedef struct _filter_ramp {
  //  u32 sr;    // sample rate
  fract32 c; // increment
  fract32 x; // target value
  fract32 y; // filtered value
  u8 sync;   // flag if target and filtered values are equal
} filter_ramp;


#endif
