#include <math.h>
#include "table.h"

/* chebyshev polynomials:
   T[0](x) = 1
   T[1](x) = x
   T[n+1](x) = 2*x*T[n](x) - T[n-1](x)
*/

//--------------------------------
//---- external function declarations

// intialize given number of bits
/*
extern void fixtable_init(fract32* tab, u32 bits) { 
  u32 i;
  
  tab->data = (fract32*)malloc(sizeof(fract32) * tab->size);
  for(i = 0; i<tab->size; i++) {
    tab->data[i] = 0;
  }
}
*/

// de-intialize
/*
extern void fixtable_deinit(fract32* tab) {
}
*/

// fill a table of given size with harmonics up to given order
// also given: decay coefficient, normalization flag
extern void fixtable_fill_harm(fract32* tab, u32 size, u8 order, f32 decay, u8 norm) {
  f32 min, max;
  f32 inc, x, amp;
  u32 samp, harm;

  amp = 1.f;
 
  // fill with zeros
  for(samp=0; samp<size; samp++) {
    tab[samp] = (fract32)0.f;
  }
  
  // fill with float values, finding maximum
  for(harm=0; harm<order; harm++) {
    inc = M_PI * 2.0 / (f32)size * (f32)(harm+1);
    x = 0.f;
    for(samp=0; samp<size; samp++) {
      tab[samp] = (fract32)( (f32)(tab[samp]) + (sinf(x) * amp) );
      if((f32)(tab[samp]) > max) { max = (f32)(tab[samp]); }
      if((f32)(tab[samp]) < min) { min = (f32)(tab[samp]); }
      x += inc;
    }
    amp *= decay;
  }
  // normalize
  if(norm) {
    min *= -1.f;
    if ( min > max ) {
      if ( min > 1.f ) {
	for(samp=0; samp<size; samp++) {
	  tab[samp] = (fract32)( (f32)(tab[samp]) / min );
	}	
      }
    } else {
      if( max > 1.f) {
	for(samp=0; samp<size; samp++) {
	  tab[samp] = (fract32)( (f32)(tab[samp]) / max );
	}
      }
    }
  }
  // convert to fract32
  for(samp=0; samp<size; samp++) {
    tab[samp] = float_to_fr32( (f32)(tab[samp]) );
  }
}

// fill a table of given size with cheby polynomial of given order
extern void fixtable_fill_cheby(fract32* tab, u32 size, u8 order) {
}
