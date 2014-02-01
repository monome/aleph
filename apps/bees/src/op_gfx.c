#include "print_funcs.h"

#include "op_gfx.h"

u8 opPlay = 0;

static u32 opPlayCount = 0;

// operator declares focus
void op_gfx_focus(void) {
  print_dbg("\r\n incrementing focus count, old value: ");
  print_dbg_ulong(opPlayCount);

  opPlayCount++;
  opPlay = 1;
}

// operator releases focus
extern void op_gfx_unfocus(void) {
  print_dbg("\r\n decrementing focus count, old value: ");
  print_dbg_ulong(opPlayCount);
  opPlayCount--;
  if(opPlayCount ==0) { 
    opPlay = 0; 
    print_dbg("\r\n , unset opPlay flag. ");
  }
}
