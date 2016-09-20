#ifndef _ALEPH_DSP_FILTER_CROSSOVER_H_
#define _ALEPH_DSP_FILTER_CROSSOVER_H_

#include "biquad.h"
typedef struct {
  biquad hpf1;
  biquad hpf2;
  biquad lpf1;
  biquad lpf2;
  fract32 out_hpf;
  fract32 out_lpf;
} crossover;
void crossover_init (crossover *xo);
void set_crossover_hpf_coeffs_from_floats(crossover *xo, float a1, float a2, float b0, float b1, float b2);
void set_crossover_lpf_coeffs_from_floats(crossover *xo, float a1, float a2, float b0, float b1, float b2);
void crossover_next(crossover *xo, fract32 x);

typedef struct {
  fract32 bands[8];
  crossover xs[7];
} crossover_8_band;
void crossover_8_band_init(crossover_8_band *xo);

void crossover_8_band_next(crossover_8_band *xo, fract32 x);

#endif
