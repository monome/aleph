#include "fix.h"
#include "ricks_tricks.h"
#include "pan.h"

#include "fract_math.h"


void hpf_init (hpf *myHpf) {
  myHpf->lastIn = 0;
  myHpf->lastOut = 0;

}
void lpf_init (lpf *myLpf) {
  myLpf->lastOut = 0;

}
void bpf_init (bpf *myBpf) {
  hpf_init(&(myBpf->hpf));
  lpf_init(&(myBpf->lpf));
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

fract32 lpf_next_dynamic_precise (lpf *myLpf, fract32 in, fract32 freq) {
  fract32 alpha = lpf_freq_calc(freq);
  fract32 out = add_fr1x32(mult_fr1x32x32(alpha, in),
			   mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha),
					  myLpf->lastOut));
  myLpf->lastOut = out;
  return out;
}

fract32 bpf_next_dynamic_precise (bpf *myBpf, fract32 in, fract32 hpf_freq, fract32 lpf_freq) {
  return lpf_next_dynamic_precise(&(myBpf->lpf),
				  hpf_next_dynamic_precise(&(myBpf->hpf),in, hpf_freq),
				  lpf_freq);
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
fract32 osc_square (fract32 phase) {
  if (phase > (1 << 30) || phase < (-1 << 30))
    return FR32_MAX;
  else
    return FR32_MIN;
}

void pitchDetector_init (pitchDetector *p) {
  p->currentPeriod = 48 << PITCH_DETECTOR_RADIX_TOTAL;
  p->period = 48 << PITCH_DETECTOR_RADIX_TOTAL;
  p->lastIn = 1;
  p->phase = 0;
  p->nsamples = 100;
  p->nFrames = 100;
  p->pitchOffset = FR32_MAX >> 2;
  /* p->pitchOffset = FR32_MAX / 2; */
  hpf_init(&(p->dcBlocker));
  lpf_init(&(p->adaptiveFilter));
}

fract32 pitchTrackOsc (pitchDetector *p) {
  //Debug uncomment the line below to force 1k tone
  /* p->currentPeriod = (48 << (PITCH_DETECTOR_RADIX_INTERNAL)); */
  p->phase +=
    (p->pitchOffset
     / shl_fr1x32(p->currentPeriod,
		  - PITCH_DETECTOR_RADIX_INTERNAL)) << 3;
  return osc(p->phase);
}

//This guy returns the current measured wave period (in subsamples)
fract32 pitchTrack (pitchDetector *p, fract32 in) {
  fract32 centreFreq = FR32_MAX / p->currentPeriod;
  in = lpf_next_dynamic_precise(&(p->adaptiveFilter), in,
				shl_fr1x32 (centreFreq,
  					     PITCH_DETECTOR_RADIX_INTERNAL + 3));
  in = hpf_next_dynamic_precise(&(p->dcBlocker),
				in,
				shl_fr1x32 (centreFreq,
					    PITCH_DETECTOR_RADIX_INTERNAL - 3));
  if (p->lastIn <= 0 && in >= 0 && p->nFrames > 12) {
    p->period = add_fr1x32(p->period,
			   min_fr1x32 (p->nFrames, 2048));
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

void radixLinSlew_init (radixLinSlew* slew, unsigned short radix) {
  slew->radix = radix;
  slew->remainder = 0;
  slew->slewRate = LINSLEW_100MS;
}

//This guy slews between target and current at *fractional* constant speed
void radixLinSlew_next (fract32* current, fract32 target, radixLinSlew *slew) {
  fract32 difference = abs_fr1x32(sub_fr1x32(target, *current));
  fract32 fract_slew = min_fr1x32(shl_fr1x32(difference, slew->radix),
				  slew->slewRate);
  fract_slew += slew->remainder;
  u32 remainderMask = ~(0xFFFFFFFF << slew->radix);
  slew->remainder = remainderMask & fract_slew;
  u32 inc = shl_fr1x32(fract_slew, - slew->radix);
  if (*current > target)
    *current -= inc;
  else if (*current < target)
    *current += inc;
}

//This guy slews between target and current at constant speed
void linSlew_next (fract32* current, fract32 target, fract32 slewRate) {
  if (*current > target)
    *current -= min_fr1x32 (abs_fr1x32(target - *current), slewRate);
  else if (*current < target)
    *current += min_fr1x32 (abs_fr1x32(target - *current), slewRate);
}

void asymLinSlew_init (asymLinSlew* slew) {
  slew->up = LINSLEW_10MS;
  slew->down = LINSLEW_100MS;
}

//This guy slews between target and current at constant speed
void asymLinSlew_next (fract32* current, fract32 target, asymLinSlew* slew) {
  if (*current > target)
    *current -= min_fr1x32 (abs_fr1x32(target - *current), slew->down);
  else if (*current < target)
    *current += min_fr1x32 (abs_fr1x32(target - *current), slew->up);
}

void radixLogSlew_init (logSlew *slew, unsigned short radix) {
  slew->radix = radix;
  slew->remainder = 0;
  slew->speed = SLEW_1S;
}

// This guy slews correctly when the slew rate is less than 1 per audio frame
void radixLogSlew_next (fract32* current, fract32 target, logSlew* slew) {
  fract32 ratio = slew->speed;
  fract32 difference = abs_fr1x32(sub_fr1x32(target, *current));
  fract32 fract_slew = mult_fr1x32x32(shl_fr1x32(difference,slew->radix),
				      ratio);
  fract_slew = add_fr1x32(fract_slew, slew->remainder);
  u32 remainderMask = ~(0xFFFFFFFF << slew->radix);
  slew->remainder = remainderMask & fract_slew;
  u32 inc = shl_fr1x32(fract_slew, - slew->radix);
  if (target > *current)
    *current = add_fr1x32(*current, inc);
  else if (target < *current)
    *current = sub_fr1x32(*current, inc);
}

// This guy slews correctly between *small* parameters
void fine_logSlew(fract32* current, fract32 target, fract32 speed) {
  fract32 ratio = FR32_MAX - speed;
  fract32 difference = sub_fr1x32(*current, target);
  fract32 inc = mult_fr1x32x32(ratio, shl_fr1x32(difference, 12));
  *current = add_fr1x32(target,
			shl_fr1x32(inc, -12));
}

//This guy slews correctly between *large* parameters
void coarse_logSlew(fract32* current, fract32 target, fract32 speed) {
  fract32 ratio = FR32_MAX - speed;
  fract32 inc = mult_fr1x32x32(ratio, sub_fr1x32(*current, target));
  *current = add_fr1x32(target, inc);
}

//This guy auto-adjusts the radix - poor man's float I guess...
void normalised_logSlew(fract32* current, fract32 target, fract32 speed) {
  fract32 ratio = speed;
  fract32 difference = sub_fr1x32(target, *current);
  int radix = norm_fr1x32(difference);
  fract32 inc = mult_fr1x32x32(ratio, shl_fr1x32(difference, radix));
  *current = add_fr1x32(*current,
			shl_fr1x32(inc, - radix));
}

void normalised_logSlew_16(fract16* current, fract16 target, fract16 speed) {
  fract16 ratio = speed;
  fract16 difference = sub_fr1x16(target, *current);
  int radix = norm_fr1x16(difference);
  fract16 inc = mult_fr1x16(ratio, shl_fr1x16(difference, radix));
  *current = add_fr1x16(*current,
			shr_fr1x16(inc, radix));
}

void trackingEnvelopeLin_init (trackingEnvelopeLin* env) {
  env->val = 0;
  asymLinSlew_init(&(env->slew));
}

fract32 trackingEnvelopeLin_next (trackingEnvelopeLin* env, fract32 in) {
  asymLinSlew_next(&(env->val), abs_fr1x32(in), &(env->slew));
  return env->val;
}

void trackingEnvelopeLog_init (trackingEnvelopeLog* env) {
  env->val = 0;
  env->up = SLEW_10MS;
  env->down = SLEW_100MS;
  env->gate = FR32_MAX / 500;
}

fract32 trackingEnvelopeLog_next (trackingEnvelopeLog* env, fract32 in) {
  fract32 target = abs_fr1x32(in);
  if (target > env->val)
    simple_slew(env->val, target, env->up);
  else if (target < env->val)
    simple_slew(env->val, target, env->down);
  return env->val;
}

fract32 soft_clip (fract32 lim, fract32 in) {
  fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX-lim, 24);
  fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
  fract32 c = mult_fr1x32x32(lim,
			     shr_fr1x32(FR32_MAX, 7) - b
			     - mult_fr1x32x32(lim, a));
  if (in >= lim) {
    fract32 ret = c;
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(b, in));
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
    return shl_fr1x32(ret, 7);
  } else if (in <= negate_fr1x32(lim)) {
    fract32 ret = negate_fr1x32(c);
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(b, in));
    ret = sub_fr1x32(ret,
		     mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
    return shl_fr1x32(ret, 7);
  } else {
    return in;
  }
}

fract32 soft_clip_asym (fract32 lim_pos, fract32 lim_neg, fract32 in) {
  fract32 lim;
  if (in >= lim_pos) {
    lim = lim_pos;
    fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX-lim, 24);
    fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
    fract32 c = mult_fr1x32x32(lim,
			       shr_fr1x32(FR32_MAX, 7) - b
			       - mult_fr1x32x32(lim, a));

    fract32 ret = c;
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(b, in));
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
    return shl_fr1x32(ret, 7);
  } else if (in <= lim_neg) {
    lim = negate_fr1x32(lim_neg);
    fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX-lim, 24);
    fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
    fract32 c = mult_fr1x32x32(lim,
			       shr_fr1x32(FR32_MAX, 7) - b
			       - mult_fr1x32x32(lim, a));

    fract32 ret = negate_fr1x32(c);
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(b, in));
    ret = sub_fr1x32(ret,
		     mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
    return shl_fr1x32(ret, 7);
  } else {
    return in;
  }
}

fract32 soft_clip_norm (fract32 lim, fract32 in) {
  fract32 b = FR32_MAX / shr_fr1x32(FR32_MAX-lim, 24);
  fract32 a = negate_fr1x32(shr_fr1x32(b, 1));
  fract32 c = mult_fr1x32x32(lim,
			     shr_fr1x32(FR32_MAX, 7) - b
			     - mult_fr1x32x32(lim, a));
  fract32 norm = FR32_MAX / shr_fr1x32(a + b + c, 20);
  if (in >= lim) {
    fract32 ret = c;
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(b, in));
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
    return shl_fr1x32(mult_fr1x32x32(ret, norm), 11);
  } else if (in <= negate_fr1x32(lim)) {
    fract32 ret = negate_fr1x32(c);
    ret = add_fr1x32(ret,
		     mult_fr1x32x32(b, in));
    ret = sub_fr1x32(ret,
		     mult_fr1x32x32(a, mult_fr1x32x32(in, in)));
    return shl_fr1x32(mult_fr1x32x32(ret, norm), 11);
  } else {
    return shl_fr1x32(mult_fr1x32x32(in, norm), 4);
  }
}

fract32 dc_block (hpf *myHpf, fract32 in) {
  fract32 in_scaled = shr_fr1x32(in, 3);
  fract32 aux = sub_fr1x32(in_scaled, myHpf->lastIn);
  myHpf->lastOut = add_fr1x32(aux, mult_fr1x32x32(0x7F600000, myHpf->lastOut));
  myHpf->lastIn = in_scaled;
  return shl_fr1x32(myHpf->lastOut, 3);
}

fract32 dc_block2 (hpf *myHpf, fract32 in) {
  fract32 in_scaled = shr_fr1x32(in, 3);
  myHpf->lastOut = mult_fr1x32x32(sub_fr1x32(add_fr1x32(in_scaled, myHpf->lastOut),
					     myHpf->lastIn),
				  0x7F600000);
  myHpf->lastIn = in_scaled;
  return shl_fr1x32(myHpf->lastOut, 3);
}
