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
// ---- echoTap
// Data structure for an 'echo tap'
// echo is position relative to write tap
// inc is tap speed along the buffer
// echoWrap is where tap starts to wrap back
// echoReset is where echo tap wraps back to
typedef struct _echoTap {
  bufferTapN* tapWr; // pointer to write head
  s32 time; // delay time in subsamples (1/256 sample)
  s32 idx_last;// last position read from (for antialiasing)

  s32 speed;
  s32 max;
  s32 min;
  //check for zero crossing

  //This flag is used to set the type of envelope.
  //No longer to be used for equal-power x-fades
  s32 shape;

  //This flag sets the boundary behaviour
  //e.g wrap, oneshot or bounce
  s32 edgeBehaviour;
  s32 fadeLength;
} echoTap;

//the maximum number of samples to use for antialiased read
#define MAX_ANTIALIAS 3

#define EDGE_ONESHOT 0 //if oneshot, then stop when grain hits a boundary
#define EDGE_WRAP 1 //if wrap, then go back to the opposite boundary
#define EDGE_BOUNCE 2 //if bounce, then flip the read head play direction when grain hits boundary

#define SHAPE_TOPHAT 0 //tophat is FR32_MpAX between max and min, 0 elsewhere
#define SHAPE_TRIANGLE 1 //triangle is linear ramp up to FR32_MAX from min to center, back down to zero from center to max
#define SHAPE_LUMP 2 //grain_lump is 1 - (abs(x)) for -1 < x > 1
  
#define SHAPE_HALFWAVE 3 //shape fadeshort is FR32_MAX until the edges, then slopes down even slower

// fixed grain envelope shapes
fract32 echoTap_envelope(echoTap *tap);
// intialize tap
extern void echoTap_init(echoTap* tap, bufferTapN* tapWr);

inline extern s32 equalPower_xfade (s32 x, s32 y, s32 pos);

// increment the index in an echo
extern void echoTap_next(echoTap* tap);

// antialiased read
inline fract32 echoTap_read_antialias(echoTap* echoTap);

// interpolated read
inline fract32 echoTap_read_interp(echoTap* tap, s32 time);

inline fract32 echoTap_read_xfade(echoTap* echoTap, s32 offset);

#endif // h guard
