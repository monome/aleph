/* fract32.h
<<<<<<< HEAD
 *
=======
 * 
>>>>>>> upstream/master
 * float-conversion and arithmetic functions for fract32 on linux.
 * these should allow compatibility with blackfin bf5xx intrinsics.
 *
 */

#include <limits.h>
#include <stdio.h>
#include <stdint.h>

#include "types.h"

#if 0
typedef int32_t fract32;
typedef int32_t fract16;
typedef float f32;
typedef double f64;
typedef int32_t s32; 
typedef int64_t s64; 
#endif

//----------------------------
//----- float <-> fract conversions

// convert fract32 to float
extern f32 fr32_to_float(fract32 x);

// convert float to fract32
extern fract32 float_to_fr32(f32 x);

//-------------------------
//----- arithmetic

// 32-bit add
extern fract32 add_fr1x32(fract32 _x, fract32 _y);

// 32-bit sub
extern fract32 sub_fr1x32(fract32 _x, fract32 _y);

// 32-bit mult with 40-bit buffer
extern fract32 mult_fr1x32x32(fract32 _x, fract32 _y);

// div

// abs
extern fract32 abs_fr1x32(fract32 _x);

// negation
extern fract32 negate_fr1x32(fract32 _x);

// minimum
extern fract32 min_fr1x32(fract32 _x, fract32 _y);

// maximum
extern fract32 max_fr1x32(fract32 _x, fract32 _y);

// left shift (with sign extension)
extern fract32 shl_fr1x32(fract32 _x, int _y);
extern fract32 shl_fr1x32_clip(fract32 _x, int _y);

// right shift (with sign extension)
extern fract32 shr_fr1x32(fract32 _x, int _y);
extern fract32 shr_fr1x32_clip(fract32 _x, int _y);

// normalize ( to [0x40000000, 0x7fffffff] or [0x80000000, 0xc0000000] )
extern int norm_fr1x32(fract32 _x);
