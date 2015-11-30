/* echoTap.h
   audio lib
   aleph

   These are 'slave' taps for creating echo effects/pitch shift
   They are indexed relative to a write head, not a buffer.
   They should do anti-aliased interpolated read

*/

#ifndef _ALEPH_SCRUBTAP_H_
#define _ALEPH_SCRUBTAP_H_
#include "fix.h"
#include "fix32.h"
#include "types.h"
#include "buffer.h"
#include "echoTap.h"
// ---- scrubTap24_8
// Data structure for an 'scrub tap'
// scrub is position relative to write tap
// inc is tap speed along the buffer
// scrubWrap is where tap starts to wrap back
// scrubReset is where scrub tap wraps back to
typedef struct _scrubTap24_8 {
  bufferTapN* tapWr; // pointer to write head
  s32 scrubTime; // delay time in subsamples (1/256 sample)
  s32 idx_last;// last position read from (for antialiasing)

  s32 playback_speed;
  s32 scrubMax;
  s32 scrubMin;
  //check for zero crossing
  u8 zero_crossing;

  //This flag is used to set the type of grain envelope
  u8 shape;

  //This flag sets the boundary behaviour
  //e.g wrap, oneshot or bounce
  u8 edge_behaviour;
} scrubTap24_8;

//the maximum number of samples to use for antialiased read
#define MAX_ANTIALIAS 3

// fixed grain envelope shapes
s32 scrubTap24_8_envelope(scrubTap24_8 *tap);
// intialize tap
extern void scrubTap24_8_init(scrubTap24_8* tap, bufferTapN* tapWr);

// increment the index in an scrub
extern void scrubTap24_8_next(scrubTap24_8* tap);

// antialiased read
extern fract32 scrubTap24_8_read_antialias(scrubTap24_8* scrubTap);

// interpolated read
extern fract32 scrubTap24_8_read_interp(scrubTap24_8* tap);

s32 scrubTap24_8_envelope(scrubTap24_8 *tap);
#endif // h guard
