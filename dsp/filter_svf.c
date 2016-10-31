/* filter_svf.c
   audio
   aleph

   digital state-variable filter for fract32 audio.
   2x oversampling for better frequency range.
 */



/////////
// test:
// #include "math.h"
////

#include "module.h"
#include "fix.h"
#include "fract_math.h"
//#include "sin_fr32.h"
#include "table.h"
#include "filter_svf.h"
#include "ricks_tricks.h"

//=======================================================
//===== static variables

//=====================================================
//===== static functions

static void filter_svf_calc_frame( filter_svf* f, fract32 in) {
  fract32 out;
  f->low = add_fr1x32(f->low, 
		      mult_fr1x32x32(f->freq, f->band));

  f->high = sub_fr1x32(sub_fr1x32(in,
				  shr_fr1x32(mult_fr1x32x32(f->rq, f->band),
					     f->rqShift)),
		       f->low);

  f->band = add_fr1x32(f->band, 
		       mult_fr1x32x32(f->freq, f->high));

}

static void filter_svf_softclip_calc_frame( filter_svf* f, fract32 in) {
  fract32 out;
  char clip_radix = 0;
  f->low = shr_fr1x32(soft_clip(FR32_MAX/2,
				shl_fr1x32(add_fr1x32(f->low,
						      mult_fr1x32x32(f->freq, f->band)),
					   clip_radix)),
		      clip_radix);
  f->high = shr_fr1x32(soft_clip(FR32_MAX/2,
				 shl_fr1x32(sub_fr1x32(sub_fr1x32(in,
								  shr_fr1x32(mult_fr1x32x32(f->rq, f->band),
									     f->rqShift)),
						       f->low),
					    clip_radix)),
		       clip_radix);
  f->band = shr_fr1x32(soft_clip(FR32_MAX/2,
				 shl_fr1x32(add_fr1x32(f->band,
						       mult_fr1x32x32(f->freq, f->high)),
					    clip_radix)),
		       clip_radix);
}

static void filter_svf_softclip_asym_calc_frame( filter_svf* f, fract32 in) {
  fract32 out;
  char clip_radix = 0;

  f->low = shr_fr1x32(soft_clip(FR32_MAX/4,
				shl_fr1x32(add_fr1x32(f->low,
						      mult_fr1x32x32(f->freq, f->band)),
					   clip_radix)),
		      clip_radix);
  f->high = shr_fr1x32(soft_clip(FR32_MAX/8,
				 shl_fr1x32(sub_fr1x32(sub_fr1x32(in,
								  shr_fr1x32(mult_fr1x32x32(f->rq, f->band),
									     f->rqShift)),
						       f->low),
					    clip_radix)),
		       clip_radix);
  f->band = shr_fr1x32(soft_clip_asym(FR32_MAX/4,
				      FR32_MAX/-8,
				      shl_fr1x32(add_fr1x32(f->band,
							    mult_fr1x32x32(f->freq, f->high)),
						 clip_radix)),
		       clip_radix);
}


//=============================================
//===== extern functions
// init
extern void filter_svf_init ( filter_svf* f ) {
  f->freq = 0;
  f->low = f->high = f->band = f->notch = 0;
  f->lowMix = f->highMix = f->bandMix = f->notchMix = f->peakMix = 0;
}

// set reciprocal of Q
extern void filter_svf_set_rq( filter_svf* f, fract32 rq) {
  // rq range is [0, 2], 
  // fract32 positive range is [0, .9999...]
  // so: move the radix to interpret rq as 2.0

  f->rqShift = norm_fr1x32(rq);
  f->rq = shl_fr1x32(rq, f->rqShift);
}


// set cutoff coefficient directly
extern void filter_svf_set_coeff( filter_svf* f, fract32 coeff ) {
  f->freq = coeff;
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

extern fract32 filter_svf_mix_outputs (filter_svf* f) {

  f->notch = add_fr1x32(f->low, f->high);

  fract32 out = mult_fr1x32x32(f->low, f->lowMix);
  out = add_fr1x32(out, mult_fr1x32x32(f->low, f->lowMix));
  out = add_fr1x32(out, mult_fr1x32x32(f->high, f->highMix));
  out = add_fr1x32(out, mult_fr1x32x32(f->band, f->bandMix));
  out = add_fr1x32(out, mult_fr1x32x32(f->notch, f->notchMix));
}

// get next value (with input)
extern fract32 filter_svf_next( filter_svf* f, fract32 in) {
  // process 2x and average
  filter_svf_calc_frame(f, in);
  fract32 out = shr_fr1x32(filter_svf_mix_outputs(f), 1);
  filter_svf_calc_frame(f, in);
  out = add_fr1x32(out, shr_fr1x32(filter_svf_mix_outputs(f), 1));
  return out;
}

extern fract32 filter_svf_lpf_next( filter_svf* f, fract32 in) {
  filter_svf_calc_frame(f, in);
  return f->low;
}

extern fract32 filter_svf_bpf_next( filter_svf* f, fract32 in) {
  filter_svf_calc_frame(f, in);
  return f->band;
}

extern fract32 filter_svf_hpf_next( filter_svf* f, fract32 in) {
  filter_svf_calc_frame(f, in);
  return f->high;
}

extern fract32 filter_svf_softclip_lpf_next( filter_svf* f, fract32 in) {
  filter_svf_softclip_calc_frame(f, in);
  return f->low;
}

extern fract32 filter_svf_softclip_bpf_next( filter_svf* f, fract32 in) {
  filter_svf_softclip_calc_frame(f, in);
  return f->band;
}

extern fract32 filter_svf_softclip_hpf_next( filter_svf* f, fract32 in) {
  filter_svf_softclip_calc_frame(f, in);
  return f->high;
}

extern fract32 filter_svf_softclip_asym_lpf_next( filter_svf* f, fract32 in) {
  filter_svf_softclip_asym_calc_frame(f, in);
  return f->low;
}

extern fract32 filter_svf_softclip_asym_bpf_next( filter_svf* f, fract32 in) {
  filter_svf_softclip_asym_calc_frame(f, in);
  return f->band;
}

extern fract32 filter_svf_softclip_asym_hpf_next( filter_svf* f, fract32 in) {
  filter_svf_softclip_asym_calc_frame(f, in);
  return f->high;
}
