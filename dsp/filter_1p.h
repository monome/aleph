/* filter_1p.h
 * null
 * aleph
 *
 * simple one-pole filters (lowpass and highpass)
 */

#ifndef _ALEPH_AUDIO_FILTER_1POLE_H_
#define _ALEPH_AUDIO_FILTER_1POLE_H_

#include "fix.h"
#include "types.h"

//-- lowpass fr32
typedef struct _filter_1p_lo {
  //  u32 sr;    // sample rate
  fract32 c; // integration coefficient
  fract32 x; // target value
  fract32 y; // filtered value
  //  u8 sync;   // flag if target and filtered values are equal
} filter_1p_lo;


//==========
//====== fr32
///---- lowpass
// initialize
extern void filter_1p_lo_init(filter_1p_lo* f, fract32 in);
// set cutoff frequency in hz
extern void filter_1p_lo_set_hz(filter_1p_lo* f, fix16 hz);
// set coefficient directly
void filter_1p_lo_set_slew(filter_1p_lo* f, fract32 slew);
// set input value
extern void filter_1p_lo_in(filter_1p_lo* f, fract32 val);
// get next filtered value
extern fract32 filter_1p_lo_next(filter_1p_lo* f);
// compare target/value
extern u8 filter_1p_sync(filter_1p_lo* f);

///---- highpass
// not using this anywhere right now.
/*
// initialize
extern void filter_1p_hi_init(filter_1p_hi* f, fract32 in);
// set cutoff frequency in hz
extern void filter_1p_hi_set_hz(filter_1p_hi* f, fix16 hz);
// set coefficient directly
void filter_1p_hi_set_slew(filter_1p_hi* f, fract32 slew);
// set input value
extern void filter_1p_hi_in(filter_1p_hi* f, fract32 val);
// get next filtered value
extern fract32 filter_1p_hi_next(filter_1p_hi* f);
*/




#endif
