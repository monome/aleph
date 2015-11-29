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
  bufferTapN* tapWr; // pointer to write head
  s32 echoTime; // delay time in subsamples (1/256 sample)
  s32 idx_last;// last position read from (for antialiasing)

  s32 playback_speed;
  s32 echoMax;
  s32 echoMin;
  //check for zero crossing
  u8 zero_crossing;

  //This flag is used to set the type of grain envelope
  u8 shape;

  //This flag sets the boundary behaviour
  //e.g wrap, oneshot or bounce
  u8 edge_behaviour;
} echoTap24_8;

//the maximum number of samples to use for antialiased read
#define MAX_ANTIALIAS 3

#define EDGE_ONESHOT 0 //if oneshot, then stop when grain hits a boundary
#define EDGE_BOUNCE 1 //if bounce, then flip the read head play direction when grain hits boundary
#define  EDGE_WRAP 2 //if wrap, then go back to the opposite boundary

#define SHAPE_TOPHAT 0 //tophat is FR32_MpAX between echoMax and echoMin, 0 elsewhere
#define SHAPE_TRIANGLE 1 //triangle is linear ramp up to FR32_MAX from echoMin to center, back down to zero from center to echoMax
#define SHAPE_LUMP 2 //grain_lump is 1 - (abs(x)) for -1 < x > 1
  
#define SHAPE_HALFWAVE 3 //shape fadeshort is FR32_MAX until the edges, then slopes down even slower

// fixed grain envelope shapes
s32 echoTap24_8_envelope(echoTap24_8 *tap);
// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr);

// increment the index in an echo
extern void echoTap24_8_next(echoTap24_8* tap);

// antialiased read
extern fract32 echoTap24_8_read_antialias(echoTap24_8* echoTap);

// interpolated read
extern fract32 echoTap24_8_read_interp(echoTap24_8* tap);

s32 echoTap24_8_envelope(echoTap24_8 *tap);
#endif // h guard
