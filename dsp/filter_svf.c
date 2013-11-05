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
#include "table.h"
#include "filter_svf.h"

//=======================================================
//===== variables

// res->q table size
#define SVF_Q_TAB_SIZE 512
#define SVF_Q_TAB_SIZE_1 511
// shift from [0,1] in fr32 to [0, tabsize) in fix16
#define SVF_Q_TAB_RSHIFT 6

// convert from fract32 input to fix16 in resonance range
// don't use it with negative numbers!
// this gives max of (fix16)63
//#define SVF_RESON_FROM_FR32(x) (fix16)((u32)(x) >> 21)

// tables for q and scale from res
static fract32 svfQTab [SVF_Q_TAB_SIZE];
static fract32 svfScaleTab [SVF_Q_TAB_SIZE];
// init flag so we can share one read-only copy
static u8 svfQscaleTabInit = 0;

//=====================================================
//===== static functions

static void svf_q_table_init(void);
//static void filter_svf_calc_freq(filter_svf* f);
static void filter_svf_calc_reson(filter_svf* f);

#if 0
static void filter_svf_calc_freq(filter_svf* f) {
  //// way too slow if it happens a lot
  /*   float tmp = fix16_to_float(f->hz) / (float)SAMPLERATE; */
  /*   tmp = sinf(M_PI * tmp) * 2.f; */
  /* // coarse approximation, no sine */
  /* /\* float tmp = fix16_to_float(f->hz) / (float)SAMPLERATE * 2.f; *\/ */
  /*   f->freq = float_to_fract32(tmp); */
  /* printf("\r\n set normalized frequency: %f", f->freq); */

  // coarse approximation, no sine:
  //    f->freq = shl_fr1x32(float_to_fr32(fix16_to_float(f->hz) / (float)SAMPLERATE), 2);
  //    f->freq = float_to_fr32(fix16_to_float(f->hz) / (float)SAMPLERATE);
  //    f->freq = (f->hz & 0xffff) << 16; //fix16_fract_trunc(f->hz);
}
#endif


static void filter_svf_calc_reson(filter_svf* f) {
  //f->rq = sqrt(1.f - atan( sqrt(fix16_to_float(f->reson))) * 2.f / M_PI );

   /* f->rq = fr32_to_float(f->reson);  */
   /* f->scale = sqrt(f->rq);  */

    /// directly?
  //  f->rq = FRACT_FIX16(f->reson));
  f->rq = f->reson;
  f->scale = FIX16_FRACT( fix16_sqrt(FRACT_FIX16(f->reson) ) );
  //// TEST
  //  f->scale = FIX16_FRACT( fix16_sqrt(FRACT_FIX16(f->scale) ) );
  //  f->scale = FIX16_FRACT( fix16_sqrt(FRACT_FIX16(f->scale) ) );

/// from table:
  //f->rq = table_lookup_idx(svfQTab, SVF_Q_TAB_SIZE, f->reson >> SVF_Q_TAB_RSHIFT);
  //  f->scale = table_lookup_idx(svfScaleTab, SVF_Q_TAB_SIZE,  f->reson >> SVF_Q_TAB_RSHIFT);
  
}

//=============================================
//===== extern functions
// init
extern void filter_svf_init ( filter_svf* f ) {
  f->freq = 0;
  f->reson = 0;
  f->low = f->high = f->band = f->notch = 0;
  f->lowMix = f->highMix = f->bandMix = f->notchMix = f->peakMix = 0;
  if(!svfQscaleTabInit) {
    svf_q_table_init();
    svfQscaleTabInit = 1;
  }
}

/* // set cutoff in hz */
/* extern void filter_svf_set_hz    ( filter_svf* f, fix16 hz ) { */
/*   if(f->hz != hz) { */
/*     f->hz = hz; */
/*     filter_svf_calc_freq(f); */
/*   } */
/* } */

// set cutoff coefficient directly
extern void filter_svf_set_coeff( filter_svf* f, fract32 coeff ) {
  if(f->freq != coeff) {
    f->freq = coeff;
    //    filter_svf_calc_freq(f);
  }
}
				       				       

// set reciprocal of Q
extern void filter_svf_set_rq( filter_svf* f, fract32 res) {
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

  /////// TEST
  /// floating point
  //float fin = fr32_to_float(in);

  /* f->low += f->freq * f->band; */
  /* if(f->low > 1.f) { printf("\r\n clipping f->low; "); f->low = 1.f; } */

  /* f->high = fin * f->scale - f->low - f->rq * f->band; */
  /* if(f->high > 1.f) { printf("\r\n clipping f->high; "); f->high = 1.f; } */

  /* f->band += f->freq * f->high; */
  /* if(f->band > 1.f) { printf("\r\n clipping f->band; "); f->band = 1.f; } */

  /* f->notch = f->high + f->low; */
  /* if(f->notch > 1.f) { printf("\r\n clipping f->notch; "); f->notch = 1.f; } */

  /* out = float_to_fr32(   f->low * fr32_to_float(f->lowMix) */
  /* 			 + f->high * fr32_to_float(f->highMix) */
  /* 			 + f->band * fr32_to_float(f->bandMix) */
  /* 			 + f->notch * fr32_to_float(f->notchMix) */
  /* 			 + (f->low - f->high) *  fr32_to_float(f->peakMix) */
  /* 			 ); */

  // TODO: could oversample if higher frequencies are desired.
  // without oversampling, max corner hz is SR/4 == 12k
  f->low = add_fr1x32(f->low, 
		      mult_fr1x32x32(f->freq, f->band));

  f->high = sub_fr1x32(
		       sub_fr1x32(
				  mult_fr1x32x32(in, f->scale),
				  mult_fr1x32x32(f->rq, f->band)
				  ),
		       f->low
		       );

  f->band = add_fr1x32(f->band, 
		       mult_fr1x32x32(f->freq, f->high) );

  f->notch = add_fr1x32(f->low, f->high);

  out = mult_fr1x32x32(f->low, f->lowMix);
  out = add_fr1x32(out, mult_fr1x32x32(f->low, f->lowMix));
  out = add_fr1x32(out, mult_fr1x32x32(f->high, f->highMix));
  out = add_fr1x32(out, mult_fr1x32x32(f->band, f->bandMix));
  out = add_fr1x32(out, mult_fr1x32x32(f->notch, f->notchMix));

  return out;
}



/*
// res: 0 - 100;
q = sqrt(1 - atan(sqrt(res)) * 2 / pi);
scale = sqrt(q);
*/
void svf_q_table_init(void) {
  u32 i;
  for(i=0; i<SVF_Q_TAB_SIZE; i++) {
    float tmp;
    // linear resonance in [0, 100]
    tmp = (float)i / (float)SVF_Q_TAB_SIZE_1 * 100.f;
    // rq
    tmp = sqrt(1.f - atan(sqrt(tmp)) * 2.f / M_PI);
    svfQTab[i] = tmp;
    // scale
    tmp = sqrt(tmp);
    svfScaleTab[i] = tmp;
  }
}
