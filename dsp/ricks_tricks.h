#ifndef RICKS_TRICKS_H
#define RICKS_TRICKS_H

#include "types.h"

#if ARCH_BFIN
#include "fix.h"
#endif


#define TWOPI 6
#define PI 3
typedef struct {
  fract32 lastIn;
  fract32 lastOut;
} hpf;

typedef struct {
  fract32 lastOut;
} lpf;


void hpf_init (hpf *myHpf);
fract32 hpf_next_dynamic (hpf *myHpf, fract32 in, fract32 freq);
void lpf_init (lpf *myLpf);
fract32 lpf_next_dynamic (lpf *myLpf, fract32 in, fract32 freq);

typedef struct {
  s32 phase;
} phasor ;

typedef struct {
  s32 cosPhase;
  s32 sinPhase;
} quadraturePhasor ;

fract32 lpf_next_dynamic (lpf *myLpf, fract32 in, fract32 freq);
s32 phasor_next_dynamic (phasor *phasor, fract32 freq);
s32 phasor_read (phasor *phasor, s32 freq);
s32 phasor_pos_next_dynamic (phasor *phasor, fract32 freq);
s32 phasor_pos_read(phasor *phasor);
void quadraturePhasor_init (quadraturePhasor *phasor);
void quadraturePhasor_pos_next_dynamic (quadraturePhasor *phasor, fract32 freq);
s32 quadraturePhasor_pos_sinRead(quadraturePhasor *phasor);
s32 quadraturePhasor_pos_sinRead(quadraturePhasor *phasor);
s32 quadraturePhasor_sinRead(quadraturePhasor *phasor);
s32 quadraturePhasor_cosRead(quadraturePhasor *phasor);
fract32 s32_flatTop_env (s32 pos, s32 fadeRatio);
fract32 osc (fract32 phase);

#define simple_slew(x, y, slew)					\
  x = add_fr1x32( (fract32) (y),				\
		  mult_fr1x32x32(sub_fr1x32(FR32_MAX,		\
					    (fract32) (slew)),	\
				 sub_fr1x32((fract32) (x),	\
					    (fract32) (y))))

#define simple_lpf(x, y, hz) \
  simple_slew(x, y, TWOPI * hzToDimensionless(hz))

fract32 max (fract32 x, fract32 y);
fract32 min (fract32 x, fract32 y);
#define SR 48000
#define hzToDimensionless(hz) ((fract32)((fract32)hz * (FR32_MAX / SR)))

#define fadeOut(x) sub_fr1x32(FR32_MAX, mult_fr1x32x32( x, x))
#define fadeIn(x) fadeOut(sub_fr1x32(FR32_MAX, x))

typedef struct {
  fract32 instantaneousPeriod;
  fract32 lastIn;
  fract32 period;
  fract32 phase;
  s32 nsamples;
  s32 nFrames;
  hpf dcBlocker;
  lpf adaptiveFilter;
} pitchDetector;

void pitchDetector_init (pitchDetector *p);
fract32 pitchTrack (pitchDetector *p, fract32 preIn);
fract32 s32_halfWave_env (fract32 pos) ;
#endif
