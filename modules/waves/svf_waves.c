/* filter_svf.c
   audio
   aleph

   digital state-variable filter for fract32 audio.
   2x oversampling for better frequency range.
 */


#include "module.h"
#include "fix.h"
#include "fract_math.h"
#include "table.h"
#include "svf_waves.h"

//=======================================================
//===== static variables

//=====================================================
//===== static functions

static fract32 filter_svf_calc_frame( filter_svf* f, fract32 in) { 
  //  fract32 out;
  f->low = add_fr1x32(f->low, 
		      mult_fr1x32x32(f->freq, f->band));

  f->high = sub_fr1x32(
		       sub_fr1x32(
				  in, 
				  shl_fr1x32(mult_fr1x32x32(f->rq, f->band), f->rqShift)
				  ),
		       f->low
		       );

  f->band = add_fr1x32(f->band, 
		       mult_fr1x32x32(f->freq, f->high) );

  //  f->notch = add_fr1x32(f->low, f->high);

  //  return out;
  return *(f->mode);
}


//=============================================
//===== extern functions
// init
extern void filter_svf_init ( filter_svf* f ) {
  f->freq = 0;
  f->low = f->high = f->band = f->notch = 0;
  //  f->lowMix = f->highMix = f->bandMix = f->notchMix = f->peakMix = 0;
  f->mode = &(f->low);
}

// set cutoff coefficient directly
extern void filter_svf_set_coeff( filter_svf* f, fract32 coeff ) {
  if(f->freq != coeff) {
    f->freq = coeff;
  }
}
				       				       

// set reciprocal of Q
extern void filter_svf_set_rq( filter_svf* f, fract32 rq) {
  /// FIXME: want additional kind of scale/tune here?
  // ok, this is kind of weird
  // rq range is [0, 2], 
  // fract32 positive range is [0, .9999...]
  // so: move the radix to interpret rq as 2.30 
  // and store lshift value
  if(rq > 0x3fffffff) {
    f->rqShift = 1;
    // clear the highest non-sign bit before shifting? (probably doesn't matter)
    f->rq = shl_fr1x32(rq & 0xbfffffff, 1);
  } else {
    f->rqShift = 0;
    f->rq = shl_fr1x32(rq, 1);
  }
}

// set output mixes
/* extern void filter_svf_set_low   ( filter_svf* f, fract16 mix) { */
/*   f->lowMix = mix; */
/* } */

/* extern void filter_svf_set_high  ( filter_svf* f, fract16 mix) { */
/*   f->highMix = mix; */
/* } */

/* extern void filter_svf_set_band  ( filter_svf* f, fract16 mix) { */
/*   f->bandMix = mix; */
/* } */

/* extern void filter_svf_set_notch ( filter_svf* f, fract16 mix) { */
/*   f->notchMix = mix; */
/* } */

/* extern void filter_svf_set_peak ( filter_svf* f, fract16 mix) { */
/*   f->peakMix = mix; */
/* } */

extern void filter_svf_set_mode( filter_svf* f, int mode) {
    // yucky hack
  f->mode = &(f->low) + (mode & 3);
}

// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in) {
  // process 2x and average
  fract32 out = shr_fr1x32(filter_svf_calc_frame(f, in), 1);
  out = add_fr1x32(out, shr_fr1x32(filter_svf_calc_frame(f, in), 1));
  return out;
}
