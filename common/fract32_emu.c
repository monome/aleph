/* fract32_emu.c
 *
<<<<<<< HEAD
 *
=======
 * 
>>>>>>> upstream/master
 */

#include <stdio.h>
#include "fract32_emu.h"

#define MASK_40 0xffffffffff

//-----------------------
//----- conversions

// fr32 to float
f32 fr32_to_float(fract32 x) {
  if(x > 0) {
    //    printf("\n? %f", (f64)x);
    return  (f32) ( (f64)x / (f64)INT_MAX);
  } else {
    return (f32) ( (f64)x / (f64)INT_MIN * -1.0 );
  }
}

// float to fr32
fract32 float_to_fr32(f32 x) {
  if(x < 0.f) {
    if(x < -1.f) {
      return INT_MIN;
    } else { 
      return (fract32)( ( (f64)(INT_MAX) + 1 ) * (f64)x);
    }
  } else {
     if(x > 1.f) {
      return INT_MAX;
    } else {
       return (fract32)((f64)INT_MAX * (f64)x);
    }
  }
}

//-------------------------
//----- arithmetic

/* Performs 32-bit addition of the two input parameters (_x+_y). */
fract32 add_fr1x32(fract32 _x, fract32 _y) {
  return _x + _y;
}

/* Performs 32-bit subtraction of the two input parameters (_x-_y). */
fract32 sub_fr1x32(fract32 _x, fract32 _y) {
  return _x - _y;
}

// 32-bit multiply
// TODO; to accurately emulate the bfin intrinsics,
// need a 40-bit accumulator with both saturating and NS flavors...
fract32 mult_fr1x32x32(fract32 _x, fract32 _y) {
    return ( ( (s64)_x * (s64)_y ) ) >> 31;
}

// 32-bit divide

// abs
fract32 abs_fr1x32(fract32 x) {
  return ( x > 0 ? x : (-1 * x));
}

// negation
fract32 negate_fr1x32(fract32 x) {
  if( x == INT_MIN ) {
    return (x+1) ^ 0xffffffff;
  } else {
    return x ^ 0xffffffff;
  }
}

// minimum
fract32 min_fr1x32(fract32 _x, fract32 _y) {
  return (_x > _y ? _y : _x);
}

// maximum
fract32 max_fr1x32(fract32 _x, fract32 _y) {
  return (_x > _y ? _x : _y);
}

// left shift (with sign extension)
fract32 shl_fr1x32(fract32 _x, int _y) {
  return _x << _y;
}

// clipping variant for compatibility only
fract32 shl_fr1x32_clip(fract32 _x, int _y) {
  return _x << _y;
}

// right shift (with sign extension)
fract32 shr_fr1x32(fract32 _x, int _y) {
  return _x >> _y;
}

// clipping variant for compatibility only
fract32 shr_fr1x32_clip(fract32 _x, int _y) {
  return _x >> _y;
}

// normalize ( to [0x40000000, 0x7fffffff] or [0x80000000, 0xc0000000] )
int norm_fr1x32(fract32 _x) {
  // don't really need...?
}
