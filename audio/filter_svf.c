/* filter_svf.c
   audio
   aleph


   a digital state-variable filter for fract32 audio.
   2x oversampling for better frequency range.
 */


#include "module.h"
#include "fix.h"
#include "fract_math.h"
#include "sin_fr32.h"

#include "filter_svf.h"

/// math ops 
// saturating
#define SVF_ADD(a, b) (add_fr1x32((a), (b)))
#define SVF_SUB(a, b) (sub_fr1x32((a), (b)))
#define SVF_MUL(a, b) (mult_fr1x32x32((a), (b)))
/// don't use this, it sucks!
#define SVF_DIV(a, b) (float_to_fr32(fr32_to_float((a)) / fr32_to_float((b))))
// non-saturating
#define SVF_MUL_NS(a, b) (mult_fr1x32x32NS((a), (b)));


//=======================================================
//===== variables
//...

//=====================================================
//===== static functions

static void filter_svf_calc_reson(filter_svf* f);

static void filter_svf_calc_freq(filter_svf* f) {
  fix16 tmp;
  tmp = fix16_div(fix16_mul(fix16_pi, f->hz), fix16_from_int((int)SAMPLERATE << 1));
  f->freq = shl_fr1x32(sin_fr32(FIX16_FRACT_TRUNC(tmp)), 1);
  // max damping coefficient for this frequency
  // this is in the algorithm but never seems to matter, unless too near nyquist anyway
  /* tmp = fix16_sub( */
  /* 		  fix16_div(fix16_two, FRACT_FIX16(f->freq)), */
  /* 		  (FRACT_FIX16(f->freq >> 1)) */
  /* 		  ); */
  /* if (tmp > fix16_two) { tmp = fix16_two; } */
  /* f->maxDamp = tmp; */
  filter_svf_calc_reson(f);
}

static void filter_svf_calc_reson(filter_svf* f) {
  // damp = 2 * (1 - (res ^ 1/4))
  // minimize with maxDamp obtained from current normalized freq

  // not using maxDamp
  f->damp = fix16_sqrt(fix16_sqrt(FRACT_FIX16(f->reson)));
  /*
  fix16 tmp;
  tmp = fix16_sqrt(fix16_sqrt(FRACT_FIX16(f->reson)));
  if (tmp > f->maxDamp) { 
    f->damp = tmp;
  } else {
    f->damp = maxDamp;
  }
  */

  
}

//=============================================
//===== extern functions
// init
extern void filter_svf_init      ( filter_svf* f ) {
  f->freq = 0;
  f->reson = 0;
  f->damp = 0;
  f->low = f->high = f->band = f->notch = 0;
  f->lowMix = f->highMix = f->bandMix = f->notchMix = f->peakMix = 0;
}

// set cutoff in hz
extern void filter_svf_set_hz    ( filter_svf* f, fix16 hz ) {
  // normalized frequency (2x oversampling)
  // 2 * sin(pi * hz / SR*2);
  if(f->hz != hz) {
    f->hz = hz;
    filter_svf_calc_freq(f);
  }
}
				       				       

// set resonance
extern void filter_svf_set_reson ( filter_svf* f, fract32 res) {
  // FIXME: find or make fr32 pow implmentation
  if(f->reson != res) {
    f->reson = res;
    // recalculate
    filter_svf_calc_reson(f);
  }
}

// set output mixes
extern void filter_svf_set_low   ( filter_svf* f, fract32 mix) {
}

extern void filter_svf_set_high  ( filter_svf* f, fract32 mix) {
}

extern void filter_svf_set_band  ( filter_svf* f, fract32 mix) {
}

extern void filter_svf_set_notch ( filter_svf* f, fract32 mix) {}

// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in) {
  fract32 out;
  // 2x oversampled: avg input with 1-sample delay 
  const fract32 avg = SVF_MUL(FR32_ONE >> 1, SVF_ADD(f->del, in));
  f->del = in;
  // input the delay-averaged value
  f->notch = SVF_SUB( avg,      SVF_MUL(f->damp, f->band) );
  f->low   = SVF_ADD( f->low,   SVF_MUL(f->freq, f->band) );
  f->high  = SVF_SUB( f->notch, f->low );
  f->band  = SVF_ADD( f->band,  SVF_MUL(f->freq, f->high) );
  // input the current value
  f->notch = SVF_SUB( in,      SVF_MUL(f->damp, f->band) );
  f->low   = SVF_ADD( f->low,   SVF_MUL(f->freq, f->band) );
  f->high  = SVF_SUB( f->notch, f->low );
  f->band  = SVF_ADD( f->band,  SVF_MUL(f->freq, f->high) );
  // mix output
  out = SVF_MUL(f->lowMix, f->low);
  out = SVF_ADD(out, SVF_MUL(f->highMix, f->high) );
  out = SVF_ADD(out, SVF_MUL(f->bandMix, f->band) );
  out = SVF_ADD(out, SVF_MUL(f->lowMix, f->notch) );
  out = SVF_ADD(out, SVF_MUL( f->peakMix, SVF_SUB( f->low, f->high) ) );
  return out;


  /* return SVF_ADD( */
  /* 		 SVF_MUL( f->peakMix, SVF_SUB( f->low, f->high) ), */
  /* 		 SVF_ADD(  */

  /* 			 SVF_ADD( */
  /* 				 SVF_MUL(f->highMix, f->high), */
  /* 				 SVF_ADD( */
  /* 					 SVF_MUL(f->bandMix, f->band), */
  /* 					 SVF_ADD( */
  /* 						 SVF_MUL(f->notchMix, f->notch) */
  /* 						 ) */
  /* 					 ) */
  /* 				 ) */
  /* 			  ) */
  /* 		 ) */
  /*   ;						 */
}
