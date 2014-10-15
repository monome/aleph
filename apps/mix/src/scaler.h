/*
  scaler.h
  aleph/apps/mix

  input scaling functions.

 */

#ifndef _ALEPH_APP_MIX_SCALER_H_
#define _ALEPH_APP_MIX_SCALER_H_

#include "types.h"

// define input ranges for our level control.
extern const s32 minLevelInput;
extern const s32 maxLevelInput;
// lookup a level control value.
// return both linear and decibel representations, by pointer
extern void scale_level(s32 val, s32* resAmp, s32* resDb);

#endif
