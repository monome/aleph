/*
  scaler.c
  
  aleph/app/mix

  this implements amplitude scaling functions,
  using tables and linear interpolation with integers.

 */

// asf
#include "print_funcs.h"

// app
#include "scaler.h"

//---------------------
//--- defines
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

//------------------
//---- variables

// level input range
const s32 minLevelInput = 0;
const s32 maxLevelInput = 0x7fff;

// table for linear amplitude (fract32)
static const s32 ampTableLinear[512] = {
#include "amp_table_linear.inc"
};
// table for decibels (16.16)
static const s32 ampTableDb[512] = {
#include "amp_table_db.inc"
};

//------------------------------
//--- static funcs

//--- ctl_interp
// fixed-point linear interpolation.
// this accepts a signed integer control argument,
// and returns an interpolated table lookup result.

// this implementation is not intended for speed,
// but to illustrate what is happening.
// all the parts of the calculation are separated for easier printing.
static s32 scaler_lerp(const s32* tab, u32 input, bool print) {
  // index
  const u32 idx = input >> INTERP_SHIFT;
  const u32 idx1 = (idx+1) & TABLE_MASK;
  // multiplier
  const u32 fr = input & INTERP_MASK;
  // endpoints
  const s32 a = tab[idx];
  const s32 b = tab[idx1];
  // take the difference in values
  const s32 dif = b - a;
  // multiply by the fractional bits and shift back to correct range
  // add this to the base index
  // note that this could break if the endpoints are very far apart!
  // but that shouldn't happen with the data we're using.
  const s32 add = (dif * fr) >> INTERP_SHIFT;
  const s32 res = a + add;

  /*
  if(print) {
    print_dbg("\r\n interpolating; input: 0x");
    print_dbg_hex(input);
    print_dbg(" dif: 0x");
    print_dbg_hex(dif);
    print_dbg(" add: 0x");
    print_dbg_hex(add);
    print_dbg(" result: 0x");
    print_dbg_hex(res);
    }
  */

  // cheat a little. 
  // at the very top end of the range, idx1 will have wrapped to 0.
  // the interpolation result will not be what we had in mind.
  // in this case let's just return the max value.
  if(idx1 == 0) {
    return a;
  } else {
    return res;
  } 
}

//--------------------------
//--- extern funcs

//--- lookup_level
// lookup a level control value.
// return both linear and decibel representations, by pointer

// (just cast input to unsigned since we've set up the ranges that way.
// if signed input made more sense (e.g. pan controls),
// we could deal with this differently.)

void scale_level(s32 val, s32* resAmp, s32* resDb) {
  *resAmp = scaler_lerp(ampTableLinear, (u32)val, 1);
  *resDb = scaler_lerp(ampTableDb, (u32)val, 0);
}
