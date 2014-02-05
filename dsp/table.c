/* table.c
 * audio
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

//--------------------------------
//---- external function declarations


////////////////////
// lookup given real index in 16.16

fract32 table_lookup_idx(fract32* tab, u32 size, fix16 idx) {
  fract32 a, b, f;
  u32 ia, ib;
  // integer part
  ia = idx >> 16;
  while(ia > (size - 1)) { ia -= (size); }
  ib = ia + 1;
  while(ib > (size - 1)) { ib -= (size); }
  a = tab[ia];
  b = tab[ib];
  f = (fract32)( (idx << 15) & 0x7fffffff );
  /// is this correct... yep
  return add_fr1x32(a, mult_fr1x32x32(f, sub_fr1x32(b, a)));
}

// (slightly) more optimized version...
fract32 table_lookup_idx_mask(fract32* tab, u32 mask, fix16 idx) {
  // packed arguments
  u32 ia = idx >> 16;
  fract32 x = tab[ia];
  // truncate arguments to 16 bits
  return add_fr1x32(x, 
		    mult_fr1x32(  
				trunc_fr1x32(sub_fr1x32(tab[(ia + 1) & mask], x)), 
				(idx & 0xffff) >> 1
				  )
		    );  
}


// lookup given normalized index in [-1, 1]
/* fract32 table_lookup_fract(fract32* tab, u32 size, fract32 phase) { */
/*   fix16 idx; */
/*   if (idx < 0) { */
/*     idx = (phase + 0x40000000) >> 1; */
/*   } else { */
/*     idx  = (phase >> 1) + 0x3fffffff; */
/*   }  */
/*   return table_lookup_idx(tab, size, idx); */
/* } */


/////////////////////


/* // fill a table of given size with all harmonics up to given order */
/* // also given: decay coefficient, normalization flag */
/* extern void table_fill_harm(fract32* tab, u32 size, u8 order, f32 decay, u8 norm) { */
/*   f32 min, max; */
/*   f32 inc, x, amp; */
/*   u32 s, h; // sample, harmonic */

/*   amp = 1.f; */
 
/*   // fill with zeros */
/*   for(s=0; s<size; s++) { */
/*     TAB_AS_FLOAT(tab, s) = 0.f; */
/*   } */
  
/*   // fill with float values, finding maximum */
/*   for(h=0; h<order; h++) { */
/*     inc = M_PI * 2.0 * (f32)(h+1) / (f32)size ; */
/*     x = 0.f; */
/*     for(s=0; s<size; s++) { */
/*       TAB_AS_FLOAT(tab, s) = TAB_AS_FLOAT(tab, s) + ( sinf(x) * amp ); */
/*       if( TAB_AS_FLOAT(tab, s) > max ) { max = TAB_AS_FLOAT(tab, s); } */
/*       if( TAB_AS_FLOAT(tab, s) < min ) { min = TAB_AS_FLOAT(tab, s); } */
/*       x += inc; */
/*     } */
/*     amp *= decay; */
/*   } */
/*   // normalize */
/*   if(norm) { */
/*     min *= -1.f; */
/*     if ( min > max ) { */
/*       if ( min > 1.f ) { */
/* 	for(s=0; s<size; s++) { */
/* 	  TAB_AS_FLOAT(tab, s) = TAB_AS_FLOAT(tab, s) / min; */
/* 	}	 */
/*       } */
/*     } else { */
/*       if( max > 1.f) { */
/* 	for(s=0; s<size; s++) { */
/* 	  TAB_AS_FLOAT(tab, s) = TAB_AS_FLOAT(tab, s) / max; */
/* 	} */
/*       } */
/*     } */
/*   } */
/*   // convert to fract32 */
/*   for(s=0; s<size; s++) { */
/*     TAB_AS_FR32(tab, s) = float_to_fr32( TAB_AS_FLOAT(tab, s) ); */
/*   } */
/* } */
