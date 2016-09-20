#include <stdio.h>
float poly_blep_posneg(float p, float dp) {
  if (p < -1.0)
    p += 1.0;
  else if (p > 1.0)
    p -= 1.0;
  float p_plus_one_by_dp = (p + 1.0) / dp;
  float start = (p < dp - 1.0)
    * (p_plus_one_by_dp*2
       - p_plus_one_by_dp*p_plus_one_by_dp - 1.0);

  float one_minus_p_by_dp = (1.0 - p) / dp;
  float end = (p > 1.0 - dp)
    * ( one_minus_p_by_dp*2
	- one_minus_p_by_dp*one_minus_p_by_dp - 1.0);
  return start - end;
};

float poly_blep_saw(float p, float dp) {
  return p - poly_blep_posneg(p, dp);
}

float poly_blep_square(float p, float dp) {
  return (p > 0) * 2.0 - 1.0 - poly_blep_posneg(p, dp)
  /* + poly_blep_posneg(p-1.0, dp) */
  + poly_blep_posneg(p+1.0, dp);
}

int main () {
  float SR = 44100;
  float freq = 1000;
  float dp = freq / SR * 2;
  float phase;
  /* for(phase=-1; phase < -1 + dp; phase += dp / 1000) */
  /*   printf ("%f\n", poly_blep_posneg(phase, dp)); */
  float sf = (float) 0x10000;
  for(phase=-1 ; phase < 1; phase += 1.0 / 1000.0)
    printf ("%f %f %f\n", sf * poly_blep_posneg(phase, dp),sf * poly_blep_saw(phase, dp), sf * poly_blep_square(phase, dp));
  
}
