#include "noise.h"

//-------------------------
//--- linear congruential

// initialize to reasonable values
extern void lcprng_reset(lcprng* o, u32 seed) {
  // from "numerical recipes"
  //  o->a = 0x19660d;
  //  o->c = 0x3c6ef35f;
  // try these...
  o->c = 1664525 ;
  o->a = 1013904223 ;
  o->x = seed;
}

// set params
extern void lcprng_set_a(lcprng* o, s32 v) {
  o->a = v;
}

extern void lcprng_set_c(lcprng* o, s32 v) {
  o->c = v;
}

extern void lcprng_set_x(lcprng* o, s32 v) {
  o->x = v;
}

// compute next
extern s32 lcprng_next(lcprng* o) {
  // allow overflow 
  o->x = o->x * o->c + o->a;
  return (s32) o->x;
}
