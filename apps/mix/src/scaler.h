/*
  scaler.h
  aleph/apps/mix

  input scaling functions.

 */

#ifndef _ALEPH_APP_MIX_SCALER_H_
#define _ALEPH_APP_MIX_SCALER_H_

#include "types.h"

// define input ranges for our level control.
extern const u32 minLevel = 0;
extern const u32 maxLevel = 0xffff;

// lookup a level control value.
// return both linear and decibel representations, by pointer
extern void lookup_level(s32 val, s32* resAmp, s32* resDb):


#endif
