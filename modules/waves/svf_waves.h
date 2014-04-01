/* filter_svf.h
   audio library
   aleph

   a digital state-variable filter for 32-bit fixed point audio.
*/

#ifndef _ALEPH_AUDIO_FILTER_SVF_H_
#define _ALEPH_AUDIO_FILTER_SVF_H_


#include "fix.h"
#include "types.h"


//==============================================
//===== types

typedef struct _filter_svf {
  fract32 freq;  // normalized frequency
  fract32 rq;    // reciprocal of q (resonance / bandwidth)
                 // range is [0, 2]
  fract32 low;   // lowpass
  fract32 band;  // bandpass
  fract32 high;
  fract32 notch;
  
  // output mix

  /// seems like these are rarely used really
  //  fract16 lowMix;
  //  fract16 highMix;
  //  fract16 bandMix;
  //  fract16 notchMix;
  //  fract16 peakMix;

  // use a single mode for output  :/
  fract32* mode;
  
  // kinda retarded, but use rshift for rq values >=1
  u8 rqShift;

} filter_svf;


//=============================================
//===== functions
// init
extern void filter_svf_init      ( filter_svf* f );
// set cutoff in hz
//extern void filter_svf_set_hz    ( filter_svf* f, fix16 hz );
// set cutoff coefficient
extern void filter_svf_set_coeff    ( filter_svf* f, fract32 coeff );

// set RQ (reciprocal of q: resonance/bandwidth)
extern void filter_svf_set_rq ( filter_svf* f, fract32 rq );
// set output mode
extern void filter_svf_set_mode   ( filter_svf* f, int mode );
// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in );

#endif // h guard
