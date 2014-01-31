#include "op_gfx.h"

extern u8 opPlay = 0;

static u32 opPlayCount = 0;

// operator declares focus
void op_gfx_focus(void) {
  opPlayCount++;
  opPlay = 1;
}

// operator releases focus
extern void op_gfx_unfocus(void) {
  opPlayCount--;
  if(opPlayCount ==0) { opPlay = 0; }
}

