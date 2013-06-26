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

// fract32 
typedef struct _filter_svf {
  // samplerate
  // u32 sr;
  // cutoff frequency in hz
  fix16 hz; 
  // normalized frequency in [0, 1]
  fract32 freq; 
  // max damping coeff at current frequency
  ////// this always seems to be a silly large value
  //  fix16 maxDamp;
  // resonance in [0, 1]
  fract32 reson; 
  // damping coefficient
  //  fix16 damp; 
#if 0
  fract32 damp; 
#else
  float damp;
#endif
  // to avoid reducing audio resolution to 16.16 (or whatever),
  /// i'm going to keep damping coeff in 0.32 
  /// and lshift by when the desired value is in [1, 2]
  /// this flag indicates that state:
  //  u8 bigDampFlag;

  // outputs
#if 0
  fract32 low;
  fract32 high;
  fract32 band;
  fract32 notch;
#else // TEST: floating point
  float low;
  float high;
  float band;
  float notch;
#endif
  //  fract32 peak;
  // 1-sample delay for input averaging
  fract32 del;
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
extern void filter_svf_set_hz    ( filter_svf* f, fix16 hz );
// set resonanc
extern void filter_svf_set_reson ( filter_svf* f, fract32 reson );
// set output mixes
extern void filter_svf_set_low   ( filter_svf* f, fract32 mix );
extern void filter_svf_set_high  ( filter_svf* f, fract32 mix );
extern void filter_svf_set_band  ( filter_svf* f, fract32 mix );
extern void filter_svf_set_notch ( filter_svf* f, fract32 mix );
extern void filter_svf_set_peak ( filter_svf* f, fract32 mix );
// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in );

#endif // h guard
