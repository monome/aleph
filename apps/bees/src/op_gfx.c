#include "print_funcs.h"

#include "op_gfx.h"

u8 opPlay = 0;

static u32 opPlayCount = 0;

// operator enabled
void op_gfx_enable(void) {
  print_dbg("\r\n incrementing gfx op count, old value: ");
  print_dbg_ulong(opPlayCount);

  opPlayCount++;
  opPlay = 1;
}

// operator disabled
extern void op_gfx_disable(void) {
  print_dbg("\r\n decrementing gfx op count, old value: ");
  print_dbg_ulong(opPlayCount);
  opPlayCount--;
  if(opPlayCount < 0) {
    opPlayCount = 0;
  }
  if(opPlayCount == 0) { 
    opPlay = 0; 
    print_dbg("\r\n , unset opPlay flag. ");
  }
}

void op_gfx_reset(void) { 
  opPlay = 0;
  opPlayCount = 0;
}
