#include "fix.h"
#include <jack/jack.h>
#include <assert.h>

fract32 clip_to_fr32(long x) {
  if(x <= (long)(fract32)FR32_MAX && x >= (long)(fract32)FR32_MIN)
    return (fract32) x;
  if(x > (fract32)FR32_MAX)
    return (fract32)FR32_MAX;
  else if (x < (fract32)FR32_MIN)
    return (fract32)FR32_MIN;
  else return 0;
}

fract32 mult_fr1x32x32_cheat(fract32 x, fract32 y) {
  return ( (((long) x) * ((long) y))) / FR32_MAX;
}

fract32 mult_fr1x32x32(fract32 x, fract32 y) {
  long res_40_bit = ((long)(x)>>12) * (((long) y) >>12);
  return clip_to_fr32((res_40_bit + (1 << 7) - 1) >> 7);
}

fract32 sub_fr1x32(fract32 x, fract32 y) {
  return clip_to_fr32((long) x - (long) y);
}

fract32 add_fr1x32(fract32 x, fract32 y) {
  return clip_to_fr32((long) x + (long) y);
}

fract32 abs_fr1x32(fract32 x) {
  if(x == FR32_MIN)
    return FR32_MAX;
  else if ( x >= 0)
    return x;
  else
    return - x;
}

fract32 min_fr1x32(fract32 x, fract32 y) {
  if (x <= y)
    return x;
  else
    return y;
}

fract32 max_fr1x32(fract32 x, fract32 y) {
  if (x >= y)
    return x;
  else
    return y;
}

fract32 negate_fr1x32(fract32 x) {
  if(x == FR32_MIN)
    return FR32_MAX;
  else if ( x >= 0)
    return - x;
  else
    return - x;
}

int norm_fr1x32(fract32 x) {
  int i;
  for (i= 0; i < 32; i++) {
    if (x > (1 << 30) ||
	x < ( -1 << 30)) {
      return i;
    }
    else {
      x = x * 2;
    }
  }
  return FR32_MAX;
}

fract32 shl_fr1x32 (fract32 x, int shft) {
  if (shft >= 0)
    return clip_to_fr32(((long) x) << shft);
  else
    return clip_to_fr32(((long) x) >> -shft);
}

fract32 shr_fr1x32 (fract32 x, int shft) {
  if (shft >= 0)
    return clip_to_fr32(((long) x) << -shft);
  else
    return clip_to_fr32(((long) x) >> shft);
}

fract32 mult_fr1x32(fract16 x, fract16 y) {
  fract32 x_32 = x;
  fract32 y_32 = y;
  return x * y;
}

void* __builtin_bfin_circptr(void* ptr, long unsigned int wordLength, void* buf, long unsigned int bufLength) {
  int idx = ptr - buf;
  idx += wordLength;
  idx = idx % bufLength;
  return buf + idx;
}


fract16 trunc_fr1x32(fract32 x) {
  x = x & 0xFFFF0000;
  return (fract16) (x >> 16);
}
