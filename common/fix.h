/* fix.h
 * aleph/common
 * 
 * some extra utilities for fixed point formatting
 */

#ifndef _ALEPH_FIX_H_
#define _ALEPH_FIX_H_

#include "libfixmath/fix16.h"
//#include "fract32_emu.h"

// use the same formatting convention as bfin fract32
// this is done in types.h though
//typedef fix16_t fix16;

// constants for printing formatted 16.16
#define FIX_DIG_LO    4 // digits after decimal
#define FIX_DIG_HI    5 // digits before decimal
#define FIX_DIG_TOTAL 11 // lo + hi + '.' + '-'

#define BSIGN(x) ((x) & 0x80000000)
#define BINV(x) ((x) ^ 0xffffffff)
//#define BABS(x) (BSIGN(x) ? BINV(x) - 1 : (x))
// hm?
#define BABS(x) (BSIGN(x) ? BINV(x) + 1 : (x))

// macros for fract32/fix16 conversion
#define FIX16_TO_U16(x) (u16)((x) >> 16)
#define FIX16_TO_S16(x) (s16)((x) >> 16)
#define S16_TO_FIX16(x) ( (fix16_t)(x) ) << 16
#define FIX16_FRACT_TRUNC(x) (fract32)(((x) & 0xffff) << 16)
#define FRACT_FIX16(x) ( BSIGN(x) ? ((x)>>15) | 0xffff8000 : (x)>>15 )

#define LIMIT_RANGE(x, max, min) ( ((x) > (max) ? (max) :  \
			     (x) < (min) ? (min) : (x) ) ) 
#define FIX16_FRACT_SAT(x) LIMIT_RANGE((x), 0xffff, 

// print to a buffer
void print_fix16(char* buf , fix16_t x);
// whole-part integer to ascii, right-justified, fixed-length
void itoa_whole(int val, char* buf, int len);
// whole-part integer to ascii, lef_justfied, return length
int itoa_whole_lj(int val, char* buf);

// fractional part to ascii, fixed length
void itoa_fract(int val, char* buf);

#endif
