/* fix.h
 * aleph/common
 * 
 * some extra utilities for fixed point formatting
 */

#ifndef _ALEPH_FIX_H_
#define _ALEPH_FIX_H_

#include "libfixmath/fix16.h"
#include "types.h"
//#include "fract32_emu.h"

// use the same formatting convention as bfin fract32
// this is done in types.h though
//typedef fix16_t fix16;

// constants for printing formatted 16.16
#define FIX_DIG_LO    5 // digits after decimal
#define FIX_DIG_HI    5 // digits before decimal
#define FIX_DIG_TOTAL 11 // lo + hi + '.' + '-'

// constants
#define FR32_MAX  0x7fffffff
#define FIX16_ONE 0x00010000

#define FRACT32_MAX     ((fract32)0x7fffffff)    /* max value of a fract32 */
#define FRACT32_MIN     ((fract32)0x80000000)    /* min value of a fract32 */

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
#define S16_TO_FIX16(x) ( (fix16_t)(x) ) << 16
#define U16_TO_FIX16(x) ( (fix16_t)(x) ) << 16
#define FIX16_FRACT_TRUNC(x) (fract32)( (( (x) & 0xffff) << 15) - ( BIT_SIGN_16(x) ? 1 : 0) )
#define FIX16_FRACT(x) FIX16_FRACT_TRUNC(x)
#define FRACT_FIX16(x) ( BIT_SIGN_16(x) ? ((x)>>15) | 0xffff0000 : (x)>>15 )

// print to a buffer
extern void print_fix16(char* buf , fix16_t x);
// whole-part integer to ascii, right-justified, fixed-length
extern void itoa_whole(int val, char* buf, int len);
// whole-part integer to ascii, lef_justfied, return length
extern int itoa_whole_lj(int val, char* buf);

// fractional part to ascii, fixed length
extern void itoa_fract(int val, char* buf);

#endif


/*


00001111222233334444555566667777

 */
