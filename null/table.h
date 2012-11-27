/* shapers.h
 * null
 * aleph
 *
 * table-lookup and waveshaping functions.
 */

#ifndef _SHAPERS_H_
#define _SHAPERS_H_


#include "fix.h"
#include "fract32_emu.h"
#include "types.h"


//-------------------------------
//---- data types

/* a fixed-point table:
   - uses signed 16.16 for phase
   - constrain table size to 2^N, then:
   -- table index = phase >> (32 -N) - 1
   -- interpolate with fract = phase << 15 (unsigned LJ -> signed RJ)
*/

//--------------------------------
//---- external function declarations

// intialize given number of bits
//extern void fixtable_init(fract32* tab, u32 bits);
// de-intialize
//extern void fixtable_deinit(fract32* tab);

// lookup given real index in [0, size-1]
#if 1
//// FIXME: this (faster) refactored form glitches verrry slightly at table endpoint (?)
inline fract32 fixtable_lookup_idx(fract32* tab, u32 size, fix16 idx) {
  fract32 a, b, f;
  u32 ia, ib;
  ia = idx >> 16;
  while(ia > (size - 1)) { ia -= (size); }
  ib = ia + 1;
  while(ib > (size - 1)) { ib -= (size); }
  a = tab[ia];
  b = tab[ib];
  f = (fract32)( (idx << 15) & 0x7fffffff );
  return add_fr1x32(a, mult_fr1x32x32(f, sub_fr1x32(b, a)));
}
#else 
inline fract32 fixtable_lookup_idx(fract32* tab, u32 size, fix16 idx) {
  s16 ia, ib;
  // fract32 fa;
  fract32 fb;
  fract32 a, b;
  
  ia = (idx >> 16) % size;
  ib = (ia + 1) % size;

  // (unsigned LJ) -> (signed RJ)
  fb = (fract32)( (idx << 15) & 0x7fffffff );
  // invert
  //  fa = sub_fr1x32(0x7fffffff, fb);
  //  return add_fr1x32(
  //		    mult_fr1x32x32(tab[ia], fa),
  //		    mult_fr1x32x32(tab[ib], fb)
  //		    );
  a = tab[ia];
  b = tab[ib];
  return add_fr1x32(a, mult_fr1x32x32(fb, sub_fr1x32(b, a)));
}
#endif
			  					   

// lookup given normalized index in [-1, 1]
inline fract32 fixtable_lookup_fract(fract32* tab, u32 size, fract32 phase) {
  fix16 idx;
  if (idx < 0) {
    idx = (phase + 0x40000000) >> 1;
  } else {
    idx  = (phase >> 1) + 0x3fffffff;
  } 
  return fixtable_lookup_idx(tab, size,idx);
}

// fill a table of given size with harmonics up to given order
// also given: decay coefficient, normalization flag
extern void fixtable_fill_harm(fract32* tab, u32 size, u8 order, f32 decay, u8 norm);
// fill a table with cheby polynomial of given order
extern void fixtable_fill_cheby(fract32* tab, u32 size, u8 order);

#endif // header guard
