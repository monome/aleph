/* table.c
 * null
 * aleph
 *
 * fixed-point wavetable functions
 */

// std
#include <math.h>
#include <stdio.h>

#include "table.h"

//----- help
typedef union { f32 fl; fract32 fr; } fu;
#define TAB_AS_FLOAT(x, i) ( (fu*)((x) + (i)) )->fl
#define TAB_AS_FR32(x, i) ( (fu*)((x) + (i)) )->fr

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

// fill a table of given size with honics up to given order
// also given: decay coefficient, normalization flag
extern void fixtable_fill_harm(fract32* tab, u32 size, u8 order, f32 decay, u8 norm) {
  f32 min, max;
  f32 inc, x, amp;
  u32 s, h; // sample, harmonic


  amp = 1.f;
 
  // fill with zeros
  for(s=0; s<size; s++) {
    // tab[s] = (fract32)0.f;
    //    ( (fu*)(tab + s) )->f = 0.f;
    TAB_AS_FLOAT(tab, s) = 0.f;
  }
  
  // fill with float values, finding maximum
  for(h=0; h<order; h++) {
    inc = M_PI * 2.0 * (f32)(h+1) / (f32)size ;
    x = 0.f;
    for(s=0; s<size; s++) {
      TAB_AS_FLOAT(tab, s) = TAB_AS_FLOAT(tab, s) + ( sinf(x) * amp );
      if( TAB_AS_FLOAT(tab, s) > max ) { max = TAB_AS_FLOAT(tab, s); }
      if( TAB_AS_FLOAT(tab, s) < min ) { TAB_AS_FLOAT(tab, s); }
      x += inc;
    }
    amp *= decay;
  }
  // normalize
  if(norm) {
    min *= -1.f;
    if ( min > max ) {
      if ( min > 1.f ) {
	for(s=0; s<size; s++) {
	  //	  tab[s] = (fract32)( (f32)(tab[s]) / min );
	  TAB_AS_FLOAT(tab, s) = TAB_AS_FLOAT(tab, s) / min;
	}	
      }
    } else {
      if( max > 1.f) {
	for(s=0; s<size; s++) {
	  //	  tab[s] = (fract32)( (f32)(tab[s]) / max );
	  TAB_AS_FLOAT(tab, s) = TAB_AS_FLOAT(tab, s) / max;
	}
      }
    }
  }
  // convert to fract32
  for(s=0; s<size; s++) {
    //    tab[s] = float_to_fr32( (f32)(tab[s]) );
    TAB_AS_FR32(tab, s) = float_to_fr32( TAB_AS_FLOAT(tab, s) );
  }


  /////////////// DEBUG
  // print
  /*
  printf("\n { ");
  for(s=0; s<size; s++) {
    printf(" 0x%08x, ", tab[s] ); 
  }
  printf("\n } ");
  */
}

// fill a table of given size with cheby polynomial of given order
extern void fixtable_fill_cheby(fract32* tab, u32 size, u8 order) {
}
