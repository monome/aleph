/* echoTap.h
   audio lib
   aleph

   These are 'slave' taps for creating echo effects/pitch shift
   They are indexed relative to a write head, not a buffer.
   They should do anti-aliased interpolated read

*/

#ifndef _ALEPH_ECHOTAP_H_
#define _ALEPH_ECHOTAP_H_
#include "fix.h"
#include "fix32.h"
#include "types.h"
#include "buffer.h"
// ---- echoTap24_8
// Data structure for an 'echo tap'
// echo is position relative to write tap
// inc is tap speed along the buffer
// echoWrap is where tap starts to wrap back
// echoReset is where echo tap wraps back to
typedef struct _echoTap24_8 {
  // pointer to write head
  bufferTapN* tapWr;
  // delay time in subsamples (1/256 sample)
  s32 echo;
  // last position read from (for antialiasing)
  s32 idx_last;
  // phase increment
  s32 inc;
  // loop position
  //s32 echoWrap;
  s32 echoMax;
} echoTap24_8;

// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr);

// increment the index in an echo
extern void echoTap24_8_next(echoTap24_8* tap);

//Check for zero crossing
extern u8 echoTap24_8_check_zero_crossing(echoTap24_8* echoTap);

// interpolated read
extern fract32 echoTap24_8_read(echoTap24_8* tap);

// interpolated read from arbitrary position
extern fract32 echoTap24_8_read_from(echoTap24_8* tap, s32 idx);

// antialiased interpolated read
extern fract32 echoTap24_8_read_antialias(echoTap24_8* tap);

// set rate
extern void echoTap24_8_set_rate(echoTap24_8 *tap, s32 inc);

// set 24.8 interp tap position directly in subsamples
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echo);

#endif // h guard
