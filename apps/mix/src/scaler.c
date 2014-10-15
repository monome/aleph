#include "scaler.h"


// bits in input
#define INPUT_BITS 15
// bits in table
#define TABLE_BITS 9
// mask with this to clip to table index range
#define TABLE_MASK ((1 << (TABLE_BITS)) - 1)
// right-shift input by this many bits to get table index
#define INTERP_SHIFT ((INPUT_BITS) - (TABLE_BITS))
// mask the input with this to get the interpolation term.
#define INTERP_MASK ((1 << (INTERP_SHIFT)) -1)

// level input range
const u32 minLevelInputt = 0;
const u32 maxLevelInput = 0x7fff;

// table for linear amplitude (fract32)
static const s32 ampTableLinear[512] = {
#include "amp_table_linear.inc";
};
// table for decibels (16.16)
static const s32 ampTableDb[512] = {
#include "amp_table_db.inc";
};

//--- ctl_interp
// fixed-point linear interpolation.
// this accepts a signed integer control argument,
// and returns an interpolated table lookup result.
// (this is neither optimally fast nor optimally clean, 
// but it illustrates the concept.)

static s32 scaler_lerp(s32* tab, u32 input) {
  // index
  const u32 idx = input >> INTERP_SHIFT;
  const u32 idx1 = (idx+1) & TABLE_MASK;
  // multiplier
  const u32 fr = input & INTERP_MASK;
  // endpoints
  const s32 a = tab[idx];
  const s32 b = tab[idx1];
  // cheat a little. 
  // at the very top end of the range, idx1 will have wrapped to 0.
  // in this case let's just skip the interpolation and return the max value.
  if(idx1 == 0) {
    return a;
  } else {
    // interpolate: multiply the difference by the fractional part,
    // and shift back by the range of our (integer) multiplier.
    return a + ( ((a - b) * fr) >> INTERP_SHIFT);
  } 
}


//--- lookup_level
// lookup a level control value.
// return both linear and decibel representations, by pointer

// (just cast input to unsigned since we've set up the ranges that way.
// if signed input made more sense (e.g. pan controls),
// we could deal with this differently.)

void lookup_level(s32 val, s32* resAmp, s32* resDb) {
  *resAmp = scaler_lerp(ampTableLinear, (u32)val);
  *resDb = scaler_lerp(ampTableDb, (u32)val);
}
