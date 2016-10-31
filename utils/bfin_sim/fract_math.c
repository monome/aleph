#include "fix.h"
#include <jack/jack.h>
#include <assert.h>

fract16 clip_to_fr16(long x) {
  if(x <= (long)(fract16)FR16_MAX && x >= (long)(fract16)FR16_MIN)
    return (fract16) x;
  if(x > (fract16)FR16_MAX)
    return (fract16)FR16_MAX;
  else if (x < (fract16)FR16_MIN)
    return (fract16)FR16_MIN;
  else return 0;
}

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

#ifndef ACCUM_RES
#define ACCUM_RES 40
#endif

#define ACCUM_PRESHIFT ((64 - ACCUM_RES) / 2)
#define ACCUM_PRE_COMPENSATE (1 << (ACCUM_PRESHIFT - 1))
#define ACCUM_POSTSHIFT (ACCUM_RES - 33)
#define ACCUM_POST_COMPENSATE (1 << (ACCUM_POSTSHIFT - 1))

fract32 mult_fr1x32x32(fract32 x, fract32 y) {
  long res_40_bit = ( ( (long)(x) + ACCUM_PRE_COMPENSATE) >> ACCUM_PRESHIFT)
    * ((((long) y) + ACCUM_PRE_COMPENSATE)>> ACCUM_PRESHIFT);
  return clip_to_fr32(((res_40_bit + ACCUM_POST_COMPENSATE) >> ACCUM_POSTSHIFT)  );
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

int norm_fr1x16(fract16 x) {
  int i;
  for (i= 0; i < 16; i++) {
    if (x > (1 << 14) ||
	x < ( -1 << 14)) {
      return i;
    }
    else {
      x = x * 2;
    }
  }
  return FR16_MAX;
}

fract32 shl_fr1x32 (fract32 x, int shft) {
  if (shft >= 0)
    return clip_to_fr32(((long) x) << shft);
  else
    return clip_to_fr32(((long) x) >> -shft);
}

fract16 shl_fr1x16 (fract16 x, int shft) {
  if (shft >= 0)
    return clip_to_fr16(((long) x) << shft);
  else
    return clip_to_fr16(((long) x) >> -shft);
}

fract32 shr_fr1x32 (fract32 x, int shft) {
  if (shft >= 0)
    return clip_to_fr32(((long) x) >> shft);
  else
    return clip_to_fr32(((long) x) << -shft);
}

fract16 shr_fr1x16 (fract16 x, int shft) {
  if (shft >= 0)
    return clip_to_fr16(((long) x) >> shft);
  else
    return clip_to_fr16(((long) x) << -shft);
}

fract32 mult_fr1x32(fract16 x, fract16 y) {
  fract32 x_32 = x;
  fract32 y_32 = y;
  return x * y;
}

fract16 trunc_fr1x32(fract32 x) {
  x = x & 0xFFFF0000;
  return (fract16) (x >> 16);
}

void* __builtin_bfin_circptr(void* ptr, long unsigned int wordLength, void* buf, long unsigned int bufLength) {
  int idx = ptr - buf;
  idx += wordLength;
  idx = idx % bufLength;
  return buf + idx;
}

fract16 add_fr1x16(fract16 _x, fract16 _y) {
  return _x + _y ;
}

fract16 sub_fr1x16(fract16 _x, fract16 _y) {
  return _x - _y ;
}

fract16 mult_fr1x16(fract16 _x, fract16 _y) {
  fract32 x = _x;
  fract32 y = _y;
  return (x * y) >> 16;
}

fract16 abs_fr1x16 (fract16 x) {
  if(x == FR16_MIN)
    return FR16_MAX;
  else if ( x >= 0)
    return x;
  else
    return - x;
}
