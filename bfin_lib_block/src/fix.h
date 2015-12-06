/* fix.h
 * aleph/common
 * 
 * some extra utilities for fixed point formatting
 */

#ifndef _ALEPH_FIX_H_
#define _ALEPH_FIX_H_

#include "libfixmath/fix16.h"
#include "types.h"

// constants
#define FR32_MAX  0x7fffffff
#define FIX16_ONE 0x00010000

#if ARCH_BFIN
#else

#define FRACT32_MAX     ((fract32)0x7fffffff)    /* max value of a fract32 */
#define FRACT32_MIN     ((fract32)0x80000000)    /* min value of a fract32 */
#endif


// FIXME these are so awful... 
#define BIT_SIGN_32(x) ((x) & 0x80000000)
#define BIT_INVERT_32(x) ( (s32)( ((x) ^ 0xffffffff) + 1) )
#define BIT_ABS_32(x) (BIT_SIGN_32(x) ? BIT_INVERT_32(x) : (x))
#define BIT_NEG_ABS_32(x) (BIT_SIGN_32(x) ? (x) : BIT_INVERT_32(x))

#define BIT_SIGN_16(x) ((x) & 0x8000)
#define BIT_INVERT_16(x) ( (s16)( ((x) ^ 0xffff) + 1) )
#define BIT_ABS_16(x) (BIT_SIGN_16(x) ? BIT_INVERT_16(x) : (x))
#define BIT_NEG_ABS_16(x) (BIT_SIGN_16(x) ? (x) : BIT_INVERT_16(x))

// macros for fract32/fix16 conversion
#define FIX16_TO_U16(x) (u16)((x) >> 16)
#define FIX16_TO_S16(x) (s16)((x) >> 16)
#define S16_TO_FIX16(x) ( (fix16_t)(x) ) << 1

#define U16_TO_FIX16(x) ( (fix16_t)(x) ) << 16
//#define FIX16_FRACT_TRUNC(x) (fract32)( (( (x) & 0xffff) << 15) - ( BIT_SIGN_32(x) ? 1 : 0) )
#define FIX16_FRACT_TRUNC(x) (shl_fr1x32( (x), 16))
#define FIX16_FRACT(x) FIX16_FRACT_TRUNC(x)
//#define FRACT_FIX16(x) ( BIT_SIGN_32(x) ? ((x)>>15) | 0xffff0000 : (x)>>15 )
#define FRACT_FIX16(x) shr_fr1x32( (x), 15)


#define fix16_add(x, y) add_fr1x32( (x), (y) )
#define fix16_sub(x, y) sub_fr1x32( (x), (y) )

#define fix16_sadd(x, y) add_fr1x32( (x), (y) )
#define fix16_ssub(x, y) sub_fr1x32( (x), (y) )


#endif
