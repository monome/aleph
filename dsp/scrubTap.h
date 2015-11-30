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
// ---- scrubTap
// Data structure for an 'scrub tap'
// scrub is position relative to write tap
// inc is tap speed along the buffer
// scrubWrap is where tap starts to wrap back
// scrubReset is where scrub tap wraps back to
typedef struct _scrubTap {
  bufferTapN* tapWr; // pointer to write head
  s32 time; // delay time in subsamples (1/256 sample)
  s32 idx_last;// last position read from (for antialiasing)

  s32 pitch;
  s32 length;
  s32 randomise;// How much to randomise scrubLength
  //check for zero crossing
  u8 zero_crossing;

  //This flag is used to set the type of grain envelope
  u8 shape;

  //This flag sets the boundary behaviour
  //e.g wrap, oneshot or bounce
  u8 edgeBehaviour;
  s32 fadeLength;
} scrubTap;

//the maximum number of samples to use for antialiased read
#define MAX_ANTIALIAS 3

// fixed grain envelope shapes
s32 scrubTap_envelope(scrubTap *tap);
// intialize tap
extern void scrubTap_init(scrubTap* tap, bufferTapN* tapWr);

// increment the index in an scrub
extern void scrubTap_next(scrubTap* tap);

// antialiased read
extern fract32 scrubTap_read_antialias(scrubTap* scrubTap);

// interpolated read
extern fract32 scrubTap_read_interp(scrubTap* tap);

s32 scrubTap_envelope(scrubTap *tap);
#endif // h guard
