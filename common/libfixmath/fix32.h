#ifndef _ALEPH_FIX32_H_
#define _ALEPH_FIX32_H_


#ifdef ARCH_BFIN // bfin
#include "fract.h"
#include "fract_math.h"
#include <fract2float_conv.h>
#else // linux
#include "fract32_emu.h"
#endif

#include "fix.h"
#include "types.h"


typedef struct _fix32 {
  s32 i;
  fract32 fr;
} fix32;



// add b to a with over/underflow protection
extern void add_fix32(fix32* a, fix32* b);

// subtract
extern void sub_fix32(fix32* a, fix32* b);

// convert fix16 to fix32
extern void fix16_to_fix32(fix16* in, fix32* out);

// wrap a 32.32 value to a positive integer range
extern void fix32_wrap_range(fix32* a, u32 upperBound);

#endif // h guard
