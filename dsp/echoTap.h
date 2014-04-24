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
  s32 echoTime;
  // last position read from (for antialiasing)
  s32 idx_last;

  s32 playback_speed;
  // loop position
  //s32 echoWrap;
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

//if oneshot, then stop when grain hits a boundary
#define EDGE_ONESHOT 0
//if bounce, then flip the read head play direction when grain hits boundary
#define EDGE_BOUNCE 1
//if wrap, then go back to the opposite boundary
#define EDGE_WRAP 2


//tophat is FR32_MAX between echoMax and echoMin, 0 elsewhere
#define SHAPE_TOPHAT 0
//triangle is linear ramp up to FR32_MAX from echoMin to center,
//back down to zero from center to echoMax
#define SHAPE_TRIANGLE 1
//grain_lump is 1 - (abs(x)) for -1 < x > 1
#define SHAPE_LUMP 2
//grain fatlump is 1 - x^2 for -1 < x < 1
#define SHAPE_FATLUMP 3
//grain obeselump is 1 - x^3 for -1 < x <1
#define SHAPE_OBESELUMP 4
//shape fadeshort is FR32_MAX until the edges, then slopes down
#define SHAPE_FADESHORT 5
//shape fadeshort is FR32_MAX until the edges, then slopes down a bit slower
#define SHAPE_FADEMEDIUM 6
//shape fadeshort is FR32_MAX until the edges, then slopes down even slower
#define SHAPE_FADELONG 7

// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr);

// increment the index in an echo
extern void echoTap24_8_next(echoTap24_8* tap);

// interpolated read
extern fract32 echoTap24_8_read(echoTap24_8* tap);

s32 echoTap24_8_envelope(echoTap24_8 *tap);
#endif // h guard
