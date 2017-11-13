/* filter_ladder.c
   audio
   aleph

   digital ladder filter for fract32 audio.
   optional oversampling for better frequency range.
 */

#include "module.h"
#include "fix.h"
#include "fract_math.h"
//#include "sin_fr32.h"
#include "table.h"
#include "filter_ladder.h"
#include "ricks_tricks.h"

// this ladder filter was designed on difference equation page 17 of
// Paul Daly's Masters thesis.  We use a different softclip algorithm
// to the tanh proposed there, which is more suited to blackfin
// primitives...
// http://www.acoustics.ed.ac.uk/wp-content/uploads/AMT_MSc_FinalProjects/2012__Daly__AMT_MSc_FinalProject_MoogVCF.pdf

extern void filter_ladder_init (filter_ladder* f) {
  int i;

  for(i=0; i < 4; i++) {
    f->filterStageOuts[i] = 0;
    f->filterStageLastIns[i] = 0;
  }
  f->clipLevel[0] = FR32_MAX >> 1;
  f->clipLevel[1] = FR32_MAX >> 2;
  f->clipLevel[2] = FR32_MAX - (FR32_MAX >> 2);
  f->clipLevel[3] = FR32_MAX >> 1;

  f->clipLevel[0] = FR32_MAX >> 2;
  f->clipLevel[1] = FR32_MAX >> 4;
  f->clipLevel[2] = FR32_MAX >> 2;
  f->clipLevel[3] = FR32_MAX >> 4;

  f->lastOutput = 0;
  f->alpha = lpf_freq_calc(1000 << 16);
  f->feedback = FR32_MAX;
  f->fbShift = 0;

}

// LTI version of 4-stage ladder topology
extern fract32 filter_ladder_lpf_next (filter_ladder* f, fract32 in) {
  int i;
  fract32 outDel = add_fr1x32(shr_fr1x32(f->filterStageOuts[3], 1),
			      shr_fr1x32(f->lastOutput, 1));
  outDel = shl_fr1x32(mult_fr1x32x32(f->feedback, outDel),
		      2 - f->fbShift);;
  f->filterStageOuts[0]
    = add_fr1x32(f->filterStageOuts[0],
		 mult_fr1x32x32(f->alpha,
				sub_fr1x32(in,
					   add_fr1x32(outDel,
						      f->filterStageOuts[0]))));
  f->lastOutput = f->filterStageOuts[3];
  for(i=1; i < 4; i++) {
    f->filterStageOuts[i]
      = add_fr1x32(f->filterStageOuts[i],
		   mult_fr1x32x32(f->alpha,
				  sub_fr1x32(f->filterStageOuts[i-1],
					     f->filterStageOuts[i])));
  }
  return f->filterStageOuts[3];
}

// LTI version of 4-stage hpf ladder topology
// NOTE: not sure this actually works, leaving it here anyway...
extern fract32 filter_ladder_hpf_next (filter_ladder* f, fract32 in) {
  int i;
  fract32 outDel = add_fr1x32(shr_fr1x32(f->filterStageOuts[3], 1),
			      shr_fr1x32(f->lastOutput, 1));
  outDel = shr_fr1x32(mult_fr1x32x32(f->feedback, outDel),
		      2 - f->fbShift);
  fract32 in_scaled = sub_fr1x32(shr_fr1x32(in, 3), outDel);
  f->filterStageOuts[0]
    = mult_fr1x32x32(add_fr1x32(in_scaled,
				sub_fr1x32(f->filterStageOuts[0],
					   f->filterStageLastIns[0])),
		     f->alpha);
  f->filterStageLastIns[0] = in_scaled;
  f->lastOutput = f->filterStageOuts[3];
  for(i=1; i < 4; i++) {
  f->filterStageOuts[i]
    = mult_fr1x32x32(add_fr1x32(f->filterStageOuts[i-1],
				sub_fr1x32(f->filterStageOuts[i],
					   f->filterStageLastIns[i])),
		     f->alpha);
  f->filterStageLastIns[i] = in_scaled;
  }
  return shl_fr1x32(f->filterStageOuts[3], 3);
}

// oversampling version of the 4-stage LTI ladder
extern fract32 filter_ladder_lpf_os_next (filter_ladder* f, fract32 in) {
  fract32 lastIn_os = add_fr1x32(shr_fr1x32(in, 1),
				 shr_fr1x32(f->lastInput, 1));

  fract32 out = shr_fr1x32(filter_ladder_lpf_next(f, lastIn_os), 1);
  out = add_fr1x32(out, shr_fr1x32(filter_ladder_lpf_next(f, in), 1));
  f->lastInput = in;
  return out;
}

// symettrical softclipping version of 4-stage ladder topology
extern fract32 filter_ladder_lpf_softclip_next (filter_ladder* f, fract32 in) {
  int i;
  fract32 outDel = add_fr1x32(shr_fr1x32(f->filterStageOuts[3], 1),
			      shr_fr1x32(f->lastOutput, 1));
  outDel = shl_fr1x32(mult_fr1x32x32(f->feedback, outDel),
		      2 - f->fbShift);;
  f->filterStageOuts[0]
    = add_fr1x32(f->filterStageOuts[0],
		 mult_fr1x32x32(f->alpha,
				sub_fr1x32(in,
					   add_fr1x32(outDel,
						      f->filterStageOuts[0]))));
  f->filterStageOuts[0] = soft_clip (f->clipLevel[0], f->filterStageOuts[0]);
  f->lastOutput = f->filterStageOuts[3];
  for(i=1; i < 4; i++) {
    f->filterStageOuts[i]
      = add_fr1x32(f->filterStageOuts[i],
		   mult_fr1x32x32(f->alpha,
				  sub_fr1x32(f->filterStageOuts[i-1],
					     f->filterStageOuts[i])));
    f->filterStageOuts[i] = soft_clip (f->clipLevel[i], f->filterStageOuts[i]);
  }
  return f->filterStageOuts[3];
}

// oversampling version of the 4-stage symmetrical softclipping ladder
extern fract32 filter_ladder_lpf_softclip_os_next (filter_ladder* f, fract32 in) {
  fract32 lastIn_os = add_fr1x32(shr_fr1x32(in, 1),
				 shr_fr1x32(f->lastInput, 1));

  fract32 out = shr_fr1x32(filter_ladder_lpf_softclip_next(f, lastIn_os), 1);
  out = add_fr1x32(out, shr_fr1x32(filter_ladder_lpf_softclip_next(f, in), 1));
  f->lastInput = in;
  return out;
}

// symettrical softclipping version of 4-stage ladder topology
extern fract32 filter_ladder_lpf_asym_next (filter_ladder* f, fract32 in) {
  int i;
  fract32 outDel = add_fr1x32(shr_fr1x32(f->filterStageOuts[3], 1),
			      shr_fr1x32(f->lastOutput, 1));
  outDel = shl_fr1x32(mult_fr1x32x32(f->feedback, outDel),
		      2 - f->fbShift);;
  f->filterStageOuts[0]
    = add_fr1x32(f->filterStageOuts[0],
		 mult_fr1x32x32(f->alpha,
				sub_fr1x32(in,
					   add_fr1x32(outDel,
						      f->filterStageOuts[0]))));
  f->filterStageOuts[0] = soft_clip_asym (f->clipLevel[0], f->clipLevelNeg[0],
					  f->filterStageOuts[0]);
  f->lastOutput = f->filterStageOuts[3];
  for(i=1; i < 4; i++) {
    f->filterStageOuts[i]
      = add_fr1x32(f->filterStageOuts[i],
		   mult_fr1x32x32(f->alpha,
				  sub_fr1x32(f->filterStageOuts[i-1],
					     f->filterStageOuts[i])));
    f->filterStageOuts[i] = soft_clip_asym (f->clipLevel[i], f->clipLevelNeg[i],
					    f->filterStageOuts[i]);
  }
  return f->filterStageOuts[3];
}

// oversampling version of the 4-stage symmetrical softclipping ladder
extern fract32 filter_ladder_lpf_asym_os_next (filter_ladder* f, fract32 in) {
  fract32 lastIn_os = add_fr1x32(shr_fr1x32(in, 1),
				 shr_fr1x32(f->lastInput, 1));

  fract32 out = shr_fr1x32(filter_ladder_lpf_asym_next(f, lastIn_os), 1);
  out = add_fr1x32(out, shr_fr1x32(filter_ladder_lpf_softclip_next(f, in), 1));
  f->lastInput = in;
  return out;
}
