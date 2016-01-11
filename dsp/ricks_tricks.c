#include "fix.h"
#include "ricks_tricks.h"
#include "pan.h"

#ifdef ARCH_BFIN
#include "fract_math.h"
#endif


void hpf_init (hpf *myHpf) {
  myHpf->lastIn = 0;
  myHpf->lastOut = 0;

}
void lpf_init (lpf *myLpf) {
  myLpf->lastOut = 0;

}

fract32 hpf_next_dynamic_precise (hpf *myHpf, fract32 in, fract32 freq) {
  //Should be 1 / (2 pi dt fc + 1)
  fract32 alpha = hpf_freq_calc(freq);
  fract32 out = add_fr1x32(mult_fr1x32x32(alpha, myHpf->lastOut),
			   mult_fr1x32x32(alpha, (sub_fr1x32(in, myHpf->lastIn))));
  myHpf->lastOut = out;
  myHpf->lastIn = in;
  return out;
}

fract32 hpf_next_dynamic (hpf *myHpf, fract32 in, fract32 freq) {
  //Should be 1 / (2 pi dt fc + 1)
  fract32 alpha =  freq * 4;
  /* alpha = (FR32_MAX / 50); */
  myHpf->lastOut = add_fr1x32 ( mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), myHpf->lastOut),
				mult_fr1x32x32(alpha, sub_fr1x32( in, myHpf->lastIn)));
  myHpf->lastIn = in ;
  return myHpf->lastOut * (FR32_MAX / freq / 4);
}


//the frequency unit is fraction of samplerate
fract32 lpf_next_dynamic (lpf *myLpf, fract32 in, fract32 freq) {
  return simple_slew(myLpf->lastOut, in, TWOPI * freq);
}

void phasor_init (phasor *phasor) {
  phasor->phase = 0;
}

s32 phasor_next_dynamic (phasor *phasor, fract32 freq) {
  phasor->phase += freq;
  return phasor->phase;
}

s32 phasor_next (phasor *phasor) {
  phasor->phase += phasor->freq;
  return phasor->phase;
}

s32 phasor_read (phasor *phasor, s32 freq) {
  return phasor->phase;
}

s32 phasor_pos_next_dynamic (phasor *phasor, fract32 freq) {
  phasor->phase += freq;
  return ((u32) phasor->phase) / 2;
}
s32 phasor_pos_read(phasor *phasor) {
  return ((u32) phasor->phase) / 2;
}

void quadraturePhasor_init (quadraturePhasor *phasor) {
  phasor->sinPhase = 0;
  phasor->cosPhase = FR32_MAX / 2;
}

void quadraturePhasor_pos_next_dynamic (quadraturePhasor *phasor, fract32 freq) {
    phasor->cosPhase += freq;
    phasor->sinPhase = phasor->cosPhase + FR32_MAX;
}

s32 quadraturePhasor_sinRead(quadraturePhasor *phasor) {
  return phasor->sinPhase;
}

s32 quadraturePhasor_cosRead(quadraturePhasor *phasor) {
  return phasor->cosPhase;
}

s32 quadraturePhasor_pos_sinRead(quadraturePhasor *phasor) {
  return (s32) (((u32) phasor->sinPhase) / (u32) 2);
}

s32 quadraturePhasor_pos_cosRead(quadraturePhasor *phasor) {
  return (s32) (((u32) phasor->cosPhase) / (u32) 2);
}

//This guy is a tophat with rounded edges
fract32 s32_flatTop_env (fract32 pos, fract32 fadeRatio) {
  pos = max_fr1x32(pos, 0);
  if (pos <= fadeRatio)
    return fadeIn(mult_fr1x32x32(FR32_MAX / fadeRatio, pos));
  else if (FR32_MAX - pos <= fadeRatio)
    return fadeIn(mult_fr1x32x32(FR32_MAX / fadeRatio, FR32_MAX - pos));
  else
    return FR32_MAX;
}

fract32 s32_halfWave_env (fract32 pos) {
  pos = max_fr1x32(pos, 0);
  if (pos <= FR32_MAX / 2)
    return fadeIn(pos * 2);
  else if (FR32_MAX - pos <= FR32_MAX / 2)
    return fadeIn((FR32_MAX - pos) * 2);
  else
    return FR32_MAX;
}

fract32 osc (fract32 phase) {
  if (phase > (1 << 30) || phase < (-1 << 30)) {
    phase = FR32_MIN - phase;
    return sub_fr1x32(shl_fr1x32(mult_fr1x32x32( phase , phase), 2),
		     FR32_MAX);
  }
  else if (phase < (1 << 30) || phase <= (-1 << 30)) {
    return sub_fr1x32(FR32_MAX,
  		      shl_fr1x32(mult_fr1x32x32( phase, phase), 2));
  }
  else
    return 0;
}

fract32 osc_triangle (fract32 phase) {
  if (phase > (1 << 30) || phase < (-1 << 30)) {
    phase = FR32_MIN - phase;
    return sub_fr1x32 (shl_fr1x32(abs_fr1x32(phase), 1),
		       FR32_MAX);
  }
  else if (phase < (1 << 30) || phase <= (-1 << 30))
    return sub_fr1x32(FR32_MAX,
		      shl_fr1x32(abs_fr1x32(phase), 1));
  else
    return 0;

}

void pitchDetector_init (pitchDetector *p) {
  p->currentPeriod = 48 << PITCH_DETECTOR_RADIX_TOTAL;
  p->period = 48 << PITCH_DETECTOR_RADIX_TOTAL;
  p->lastIn = 1;
  p->phase = 0;
  p->nsamples = 100;
  p->nFrames = 100;
  hpf_init(&(p->dcBlocker));
  lpf_init(&(p->adaptiveFilter));
}

fract32 pitchTrackOsc (pitchDetector *p) {
  //Debug uncomment the line below to force 1k tone
  /* p->currentPeriod = (48 << (PITCH_DETECTOR_RADIX_INTERNAL)); */
  p->phase += FR32_MAX / shl_fr1x32(p->currentPeriod,
				    -1 - PITCH_DETECTOR_RADIX_INTERNAL);
  return osc(p->phase);
}

//This guy returns the current measured wave period (in subsamples)
fract32 pitchTrack (pitchDetector *p, fract32 in) {
  in = hpf_next_dynamic(&(p->dcBlocker),
  			in,
  			hzToDimensionless(50));
  in = lpf_next_dynamic (&(p->adaptiveFilter), in,
			 pan_lin_mix(hzToDimensionless(5000),
				     shl_fr1x32(FR32_MAX / p->currentPeriod,
						PITCH_DETECTOR_RADIX_INTERNAL - 4),
				     (200 << 23)
				     //this fudge factor is required!
				     //if you key lpf straight off detected frequency
				     //the whole thing blows up. Probably can further improve
				     //tracking by shifting RADIX_INTERNAL up/down, then
				     //fiddling with panning factor.
				     //Another approach would be more accurate calculation of
				     // 1 / (2 pi dt fc + 1)
				     ));
  if (p->lastIn <= 0 && in >= 0 && p->nFrames > 24) {
    p->period = add_fr1x32(p->period,
			   min_fr1x32 (p->nFrames, 1024));
    p->nFrames = 0;
    p->nsamples += 1;
    if (p->nsamples >= (1 << PITCH_DETECTOR_RADIX_INTERNAL)) {
      p->currentPeriod = p->period ;
      p->period = 0;
      p->nsamples = 0;
    }
  }
  p->nFrames +=1;
  p->lastIn = in;
  return (shl_fr1x32(p->currentPeriod, PITCH_DETECTOR_RADIX_EXTERNAL - PITCH_DETECTOR_RADIX_INTERNAL));
}

// 4-point, 3rd-order B-spline (x-form) from http://yehar.com/blog/wp-content/uploads/2009/08/deip.pdf
float interp_bspline_float (float x, float _y, float y, float y_, float y__) {
  float ym1py1 = _y+y_;
  float c0 = 1/6.0*ym1py1 + 2/3.0*y;
  float c1 = 1/2.0*(y_-_y);
  float c2 = 1/2.0*ym1py1 - y;
  float c3 = 1/2.0*(y-y_) + 1/6.0*(y__-_y);
  return ((c3*x+c2)*x+c1)*x+c0;
}

fract32 interp_bspline_fract32 (fract32 x, fract32 _y, fract32 y, fract32 y_, fract32 y__) {
  fract32 ym1py1 = _y / 256 + y_ / 256;
  fract32 c0 = ym1py1/6 + (y/128)/3;
  fract32 c1 = (y_-_y)/512;
  fract32 c2 = ym1py1/2 - y/256;
  fract32 c3 = (y-y_)/512 + ((y__-_y)/512)/3;
  return
    256 * (c0 + mult_fr1x32x32(x,
			       (c1 +
				mult_fr1x32x32((mult_fr1x32x32(c3,x) + c2),x))));
}
