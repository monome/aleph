/* filter_svf.h
   audio library
   aleph

   a digital state-variable filter for 32-bit fixed point audio.
*/

#ifndef _ALEPH_AUDIO_FILTER_SVF_H_
#define _ALEPH_AUDIO_FILTER_SVF_H_


#include "fix.h"
#include "types.h"

// define false for float (sorry)
#define SVF_FP 0

//==============================================
//===== types

typedef struct _filter_svf {
  //  fix16 hz;      // cutoff in hz
  // coefficiet
  fract32 coeff;
  fract32 reson; // resonance in [0, 1 ?? ]
  // runtime coeffs

  fract32 freq;     // normalized frequency
  fract32 rq;    // reciprocal of q (resonance / bandwidth)
  // fix16 rq;    // reciprocal of q (resonance / bandwidth)
  fract32 scale; // scale input by resonance
  fract32 low;   // lowpass
  fract32 band;  // bandpass
  fract32 high;
  fract32 notch;
  
  /* float freq;     // normalized frequency */
  /* float rq;    // reciprocal of q (resonance / bandwidth) */
  /* float scale; // scale input by resonance */
  /* float low;   // lowpass */
  /* float band;  // bandpass */
  /* float high; */
  /* float notch; */
  
  // output mix
  fract32 lowMix;
  fract32 highMix;
  fract32 bandMix;
  fract32 notchMix;
  fract32 peakMix;
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
// set output mixes
extern void filter_svf_set_low   ( filter_svf* f, fract32 mix );
extern void filter_svf_set_high  ( filter_svf* f, fract32 mix );
extern void filter_svf_set_band  ( filter_svf* f, fract32 mix );
extern void filter_svf_set_notch ( filter_svf* f, fract32 mix );
extern void filter_svf_set_peak ( filter_svf* f, fract32 mix );
// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in );

#endif // h guard
