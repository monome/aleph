#include "fix.h"
#include "ricks_tricks.h"

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
fract32 hpf_next_dynamic (hpf *myHpf, fract32 in, fract32 freq) {
  fract32 alpha =  freq * 4;
  /* alpha = (FR32_MAX / 50); */
  myHpf->lastOut = add_fr1x32 ( mult_fr1x32x32(sub_fr1x32(FR32_MAX, alpha), myHpf->lastOut),
				mult_fr1x32x32(alpha, sub_fr1x32( in, myHpf->lastIn)));
  myHpf->lastIn = in ;
  return myHpf->lastOut * (FR32_MAX / freq / 4);
}


//the frequency unit is fraction of samplerate
fract32 lpf_next_dynamic (lpf *myLpf, fract32 in, fract32 freq) {
  fract32 slew = sub_fr1x32(FR32_MAX, TWOPI * freq);
  return simple_slew(myLpf->lastOut, in, slew);
}

s32 phasor_next_dynamic (phasor *phasor, fract32 freq) {
  phasor->phase += freq;
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
  pos = max(pos, 0);
  if (pos <= fadeRatio)
    return fadeIn(mult_fr1x32x32(FR32_MAX / fadeRatio, pos));
  else if (FR32_MAX - pos <= fadeRatio)
    return fadeIn(mult_fr1x32x32(FR32_MAX / fadeRatio, FR32_MAX - pos));
  else
    return FR32_MAX;
}

fract32 s32_halfWave_env (fract32 pos) {
  pos = max(pos, 0);
  if (pos <= FR32_MAX / 2)
    return fadeIn(pos * 2);
  else if (FR32_MAX - pos <= FR32_MAX / 2)
    return fadeIn((FR32_MAX - pos) * 2);
  else
    return FR32_MAX;
}

fract32 osc (fract32 phase) {
  if (phase > FR32_MAX / 2 || phase <= (fract32) FR32_MIN / 2) {
    phase = FR32_MIN - phase;
    return sub_fr1x32(4 * mult_fr1x32x32( phase , phase),
		     FR32_MAX);
  }
  else
    return sub_fr1x32(FR32_MAX,
		      4 * mult_fr1x32x32( phase, phase));
}

fract32 min (fract32 x, fract32 y) {
  if (x < y)
    return x;
  else
    return y;
}

fract32 max (fract32 x, fract32 y) {
  if (x > y)
    return x;
  else
    return y;
}

void pitchDetector_init (pitchDetector *p) {
  p->instantaneousPeriod = 48;
  p->period = 48;
  p->lastIn = 0;
  p->phase = 0;
  p->nsamples = 0;
  hpf_init(&(p->dcBlocker));
  lpf_init(&(p->adaptiveFilter));
}

fract32 pitchTrack (pitchDetector *p, fract32 preIn) {
  fract32 in = hpf_next_dynamic(&(p->dcBlocker), preIn, hzToDimensionless(50));
  in = lpf_next_dynamic (&(p->adaptiveFilter), in , FR32_MAX / p->period);
  if (p->lastIn <= 0 && in >= 0 && p->nsamples > 10.0) {
    simple_slew ((p->period), (max (min((fract32) p->nsamples,
				     FR32_MAX / hzToDimensionless(50)),
				    FR32_MAX / hzToDimensionless(2000))),
		 (FR32_MAX / 256));
    p->nsamples = 0;
  }
  p->nsamples += 1;
  simple_slew (p->instantaneousPeriod, p->period, 128);
  /* p->instantaneousPeriod = p->period; */
  p->phase += (FR32_MAX / p->instantaneousPeriod) * 2;
  p->lastIn = in;
  return osc(p->phase);
}
