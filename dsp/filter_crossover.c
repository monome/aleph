#include "filter_crossover.h"

void crossover_init (crossover *xo) {
  biquad_init(&(xo->hpf1));
  biquad_init(&(xo->hpf2));
  biquad_init(&(xo->lpf1));
  biquad_init(&(xo->lpf2));
}
void set_crossover_hpf_coeffs_from_floats(crossover *xo, float a1, float a2, float b0, float b1, float b2) {
  set_biquad_coeffs_from_floats(&(xo->hpf1), a1, a2, b0, b1, b2);
  set_biquad_coeffs_from_floats(&(xo->hpf2), a1, a2, b0, b1, b2);
}
void set_crossover_lpf_coeffs_from_floats(crossover *xo, float a1, float a2, float b0, float b1, float b2) {
  set_biquad_coeffs_from_floats(&(xo->lpf1), a1, a2, b0, b1, b2);
  set_biquad_coeffs_from_floats(&(xo->lpf2), a1, a2, b0, b1, b2);
}

void crossover_next(crossover *xo, fract32 x) {
  xo->out_hpf = biquad_next(&(xo->hpf1), biquad_next(&(xo->hpf2), x));
  xo->out_lpf = biquad_next(&(xo->lpf1), biquad_next(&(xo->lpf2), x));
}

void crossover_8_band_init(crossover_8_band *xo) {
  int i;
  for (i=0; i < 7; i++) {
    crossover_init(&(xo->xs[i]));
    xo->bands[i] = 0;
  }
  xo->bands[7] = 0;

set_crossover_lpf_coeffs_from_floats(&(xo->xs[0]),-1.9065387813897672,0.9107153898957953,0.001044152126507015,0.00208830425301403,0.001044152126507015);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[0]),-1.9065387813897672,0.9107153898957953,0.9543135428213906,-1.9086270856427812,0.9543135428213906);
set_crossover_lpf_coeffs_from_floats(&(xo->xs[1]),-1.8442732982346497,0.8555520978913795,0.0028196999141824833,0.005639399828364967,0.0028196999141824833);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[1]),-1.8442732982346497,0.8555520978913795,0.9249563490315073,-1.8499126980630145,0.9249563490315073);
set_crossover_lpf_coeffs_from_floats(&(xo->xs[2]),-1.7409758381571883,0.7708734552545649,0.0074744042743441486,0.014948808548688297,0.0074744042743441486);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[2]),-1.7409758381571883,0.7708734552545649,0.8779623233529383,-1.7559246467058767,0.8779623233529383);
set_crossover_lpf_coeffs_from_floats(&(xo->xs[3]),-1.570882267555086,0.6479086592929016,0.019256597934453938,0.038513195868907876,0.019256597934453938);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[3]),-1.570882267555086,0.6479086592929016,0.804697731711997,-1.609395463423994,0.804697731711997);
set_crossover_lpf_coeffs_from_floats(&(xo->xs[4]),-1.2948879445879682,0.4853876226346263,0.04762491951166454,0.09524983902332908,0.04762491951166454);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[4]),-1.2948879445879682,0.4853876226346263,0.6950688918056486,-1.3901377836112971,0.6950688918056486);
set_crossover_lpf_coeffs_from_floats(&(xo->xs[5]),-0.8569004011546622,0.30449627547502833,0.11189896858009156,0.22379793716018312,0.11189896858009156);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[5]),-0.8569004011546622,0.30449627547502833,0.5403491691574227,-1.0806983383148454,0.5403491691574227);
set_crossover_lpf_coeffs_from_floats(&(xo->xs[6]),-0.1712407118376786,0.17675670276251634,0.2513789977312094,0.5027579954624188,0.2513789977312094);
set_crossover_hpf_coeffs_from_floats(&(xo->xs[6]),-0.1712407118376786,0.17675670276251634,0.33699935365004874,-0.6739987073000975,0.33699935365004874);
}

void crossover_8_band_next(crossover_8_band *xo, fract32 x) {
  int i;
  crossover_next(&(xo->xs[0]), x);
  xo->bands[0] = xo->xs[0].out_lpf;
  for(i=1; i < 7; i++) {
    crossover_next(&(xo->xs[i]), xo->xs[i-1].out_hpf);
    xo->bands[i] = xo->xs[i].out_lpf;
  }
  xo->bands[7] = xo->xs[6].out_hpf;
  /* xo->bands[1] = xo->xs[0].out_hpf; */
}

