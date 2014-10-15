#include "scaler.h"

const u32 minLevel = 0;
const u32 maxLevel = 0xffff;

// tables
const s32 ampTableLinear[512] = {
  #include 
};
// tables
const s32 ampTableDb[512] = {
};

// helper: fixed-point linear interpolation.
// this accepts a full-scale signed integer control argument,
// and returns an interpolated table lookup result.
// it's not particularly fast, just demonstrating the concepts..
static s32 ctl_interp(s32* table, u32 bits, u32 idx) {
  // this will be a bitmask applied to table index
  const u32 mask = 1 << bits;
  // the endpoints of our interpolation
}


// lookup a level control value.
// return both linear and decibel representations, by pointer
void lookup_level(s32 val, s32* resAmp, s32* resDb) {
  
}
