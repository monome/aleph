/* shapers.h
 * null
 * aleph
 *
 * table-lookup and waveshaping functions.
 */

#ifndef _TABLE_H_
#define _TABLE_H_

#include "../common/fix.h"

#ifdef ARCH_BFIN
#include "fract_math.h"
#include <fract2float_conv.h>
#else
#include "fract32_emu.h"
#endif

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
//// FIXME: how can i inline these without multiple definition errors?
/*
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
*/
extern fract32 table_lookup_idx(fract32* tab, u32 size, fix16 idx);
extern fract32 table_lookup_fract(fract32* tab, u32 size, fract32 phase);

// fill a table of given size with harmonics up to given order
// also given: decay coefficient, normalization flag
extern void table_fill_harm(fract32* tab, u32 size, u8 order, f32 decay, u8 norm);
// fill a table with cheby polynomial of given order
extern void table_fill_cheby(fract32* tab, u32 size, u8 order);

#endif // header guard
