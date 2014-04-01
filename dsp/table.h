/* shapers.h
 * null
 * aleph
 *
 * table-lookup and waveshaping functions.
 */

#ifndef _TABLE_H_
#define _TABLE_H_

#include "fix.h"

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
 
// lookup given 16.16 index in [0, size-1]
extern fract32 table_lookup_idx(fract32* tab, u32 size, fix16 idx);

// assume size is 2^N and use bitmask for faster wrapping
extern fract32 table_lookup_idx_mask(fract32* tab, u32 mask, fix16 idx);


// lookup given normalized index in [-1, 1]
// extern fract32 table_lookup_fract(fract32* tab, u32 size, fract32 phase);

// fill a table of given size with harmonics up to given order
// also given: decay coefficient, normalization flag
//extern void table_fill_harm(fract32* tab, u32 size, u8 order, f32 decay, u8 norm);
// fill a table with cheby polynomial of given order
//extern void table_fill_cheby(fract32* tab, u32 size, u8 order);

#endif // header guard
