/* filter_svf.c
   audio
   aleph


   a digital state-variable filter for fract32 audio.
   2x oversampling for better frequency range.
 */



/////////
// test:
#include "math.h"
////

#include "module.h"
#include "fix.h"
#include "fract_math.h"
#include "sin_fr32.h"

#include "filter_svf.h"

// samplerate in fix16
#define SR_FIX16 SAMPLERATE << 16

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
  // normalized frequency (2x oversampling)
  // f = 2 * sin(pi * hz / SR*2);
#if 0
  fix16 tmp;
  tmp = fix16_div(f->hz, SR_FIX16);
  tmp >>= 1; // oversampled, so SRx2
  if (tmp > 0x4000) { tmp = 0x4000; }
  tmp = fix16_mul(tmp, fix16_pi);  
  f->freq = shl_fr1x32(sin_fr32(FIX16_FRACT_TRUNC(tmp)), 1);
#else 
  // TEST: floating point
  float tmp = fix16_to_float(f->hz) / ((float)SAMPLERATE * 2.f);
  printf("\r\n freq tmp: %f ", tmp);
  if (tmp > 0.25f) { tmp = 0.25f; }
  printf("\r\n freq tmp: %f ", tmp);
  tmp *= M_PI;
  printf("\r\n freq tmp: %f ", tmp);
  tmp = sinf(tmp) * 2.f;
  printf("\r\n freq tmp: %f ", tmp);
  if(tmp > 1.f) { 
    tmp = 1.f;
    printf("\r\n clamped normalized freq.");
  }
  f->freq = sinf(tmp) * 2.f;
  printf("\r\n set normalized unit frequency to %f", f->freq);
#endif
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
#if 0
  fix16 tmp = fix16_sqrt(fix16_sqrt(FRACT_FIX16(f->reson)));
  tmp = fix16_sub(fix16_one, tmp);
  tmp <<= 1;
  // not using maxDamp
  /// FIXME: this will limit our resonance, or even overflow.
  // in practice the damping coefficient will slighlty exceed 1.0 for high resonance.
  f->damp = FIX16_FRACT_TRUNC(tmp);
  printf("\r\n calculated damping factor 0x%x for resonance 0x%x", f->damp, f->reson);
#else
  // TEST: floating point
  float tmp;
  tmp = fr32_to_float(f->reson);
  printf("\r\n reson tmp: %f ", tmp);
  tmp = 2.f * (1.f - powf(tmp, 0.25));
  printf("\r\n reson tmp: %f ", tmp);
  if (tmp > 1.f) {
    tmp = 1.f;
    printf("\r\n clamped damping coefficient.");
  }
  f->damp = tmp;
    printf("\r\n calculated damping factor %f for resonance 0x%x", f->damp, f->reson);
#endif
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
  f->del = 0;
}

// set cutoff in hz
extern void filter_svf_set_hz    ( filter_svf* f, fix16 hz ) {
  if(f->hz != hz) {
    f->hz = hz;
    filter_svf_calc_freq(f);
  }
}
				       				       

// set resonance
extern void filter_svf_set_reson ( filter_svf* f, fract32 res) {
  if(f->reson != res) {
    f->reson = res;
    // recalculate
    filter_svf_calc_reson(f);
  }
}

// set output mixes
extern void filter_svf_set_low   ( filter_svf* f, fract32 mix) {
  f->lowMix = mix;
}

extern void filter_svf_set_high  ( filter_svf* f, fract32 mix) {
  f->highMix = mix;
}

extern void filter_svf_set_band  ( filter_svf* f, fract32 mix) {
  f->bandMix = mix;
}

extern void filter_svf_set_notch ( filter_svf* f, fract32 mix) {
  f->notchMix = mix;
}

extern void filter_svf_set_peak ( filter_svf* f, fract32 mix) {
  f->peakMix = mix;
}

// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in) {
  fract32 out;


#if 0

/*   // 2x oversampled: avg input with 1-sample delay */
/*   fract32 avg = SVF_ADD(f->del, in); */
/*   avg >>= 1; */
/*   f->del = in; */
/*   // input the delay-averaged value */
/* #if 0 */
/*   f->notch = SVF_SUB( avg,      SVF_MUL(f->damp, f->band) ); */
/*   f->low   = SVF_ADD( f->low,   SVF_MUL(f->freq, f->band) ); */
/*   f->high  = SVF_SUB( f->notch, f->low ); */
/*   f->band  = SVF_ADD( f->band,  SVF_MUL(f->freq, f->high) ); */
/* #endif */
/*   // input the current value */
/*   f->notch = SVF_SUB( in,      SVF_MUL(f->damp, f->band) ); */
/*   f->low   = SVF_ADD( f->low,   SVF_MUL(f->freq, f->band) ); */
/*   f->high  = SVF_SUB( f->notch, f->low ); */
/*   f->band  = SVF_ADD( f->band,  SVF_MUL(f->freq, f->high) ); */
/*   // mix output */
/*   out = SVF_MUL(f->lowMix, f->low); */
/*   out = SVF_ADD(out, SVF_MUL(f->highMix, f->high) ); */
/*   out = SVF_ADD(out, SVF_MUL(f->bandMix, f->band) ); */
/*   out = SVF_ADD(out, SVF_MUL(f->lowMix, f->notch) ); */
/*   out = SVF_ADD(out, SVF_MUL( f->peakMix, SVF_SUB( f->low, f->high) ) ); */

#else
  /////// TEST
  /// floating point
  float fin = fr32_to_float(in);
  f->notch = fin - f->damp * f->band;
  f->low = f->low + f->freq * f->band;
  f->high = f->notch - f->low;
  f->band = f->freq * f->high + f->band;

  out = float_to_fr32(   f->low * fr32_to_float(f->lowMix)
			 + f->high * fr32_to_float(f->highMix)
			 + f->band * fr32_to_float(f->bandMix)
			 + f->notch * fr32_to_float(f->notchMix)
			 + (f->low - f->high) *  fr32_to_float(f->peakMix)
			 );

  #endif

  return out;
}
