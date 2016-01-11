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
#include "pan.h"
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

void echoTap_next(echoTap* tap);

// fixed grain envelope shapes
fract32 echoTap_envelope(echoTap *tap);
// intialize tap
extern void echoTap_init(echoTap* tap, bufferTapN* tapWr);

// interpolated read
static inline fract32 echoTap_read_interp(echoTap* echoTap, s32 time) {
    s32 loop = echoTap->tapWr->loop << 8;
    s32 idx = ((echoTap->tapWr->idx << 8) + loop - time) % loop;

    u32 samp1_index = idx / 256;
    u32 samp2_index = ( (idx + 256) % loop ) / 256 ;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index ];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index ];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);

    fract32 pre_fader = pan_lin_mix(samp1, samp2, inter_sample)   ;
    /* fract32 fader = echoTap_envelope(echoTap); */
    /* fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader); */
    return pre_fader;
}

//#!gnuplot
//plot cos(x*pi/2), sin(x*pi/2), 1 - x**2, 1 - (1 - x)**2
//fades x into y as pos slides from 0 to FR32_MAX
static inline s32 equalPower_xfade (fract32 x, fract32 y, fract32 pos) {
  fract32 pos_2 = mult_fr1x32x32(pos, pos);
  fract32 amp_x = sub_fr1x32(FR32_MAX,  pos_2);
  fract32 pos_bar = sub_fr1x32(FR32_MAX,  pos);
  fract32 amp_y = sub_fr1x32(FR32_MAX,  mult_fr1x32x32(pos_bar, pos_bar));
  return add_fr1x32(mult_fr1x32x32(amp_x, x),
		    mult_fr1x32x32(amp_y, y));
}

static inline fract32 echoTap_boundToFadeRatio (echoTap* tap, s32 unbounded) {
  return (fract32) (max_fr1x32 (0,
			     min_fr1x32 (tap->fadeLength, unbounded))
		    * (FR32_MAX / (max_fr1x32 (1, tap->fadeLength))));
}

fract32 echoTap_read_xfade(echoTap* echoTap, s32 offset);

#endif // h guard
