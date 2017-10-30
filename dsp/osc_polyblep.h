#ifndef OSC_POLYBLEP_H
#define OSC_POLYBLEP_H
#include "types.h"

fract16 saw_polyblep (fract32 p, fract32 dp);
fract16 square_polyblep (fract32 p, fract32 dp);
fract16 sine_polyblep (fract32 phase);
fract16 triangle_polyblep (fract32 phase);
#endif
