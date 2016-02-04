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
#include "noise.h"
#include "ricks_tricks.h"

// ---- scrubTap
// Data structure for an 'scrub tap'
// scrub is position relative to write tap
// inc is tap speed along the buffer
// scrubWrap is where tap starts to wrap back
// scrubReset is where scrub tap wraps back to
typedef struct _scrubTap {
  echoTap* echoTap; // pointer to echoTap
  quadraturePhasor quadraturePhasor;
  s32 length;
  s32 frequency;
} scrubTap;

//the maximum number of samples to use for antialiased read

// intialize tap
extern void scrubTap_init(scrubTap* tap, echoTap* echoTap);

// increment the index in an scrub
extern void scrubTap_next(scrubTap* tap);

extern fract32 scrubTap_read_xfade(scrubTap* scrubTap);

#endif // h guard
