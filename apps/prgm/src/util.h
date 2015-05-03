//util.h
//aleph-prgm-avr32

#ifndef _ALEPH_AVR32_APP_PRGM_UTIL_H_
#define _ALEPH_AVR32_APP_PRGM_UTIL_H_

#include "fix.h"
#include "types.h"
#include <string.h>

#include <limits.h>
#include <stdio.h>
#include <stdint.h>

#define MASK_40 0xffffffffff

//extern function definitions
//convert fract32 to float
extern f32 fr32_to_float(fract32 x);

//convert float to fract32
extern fract32 float_to_fr32(f32 x);

//32-bit add
extern fract32 add_fr1x32(fract32 _x, fract32 _y);

//32-bit sub
extern fract32 sub_fr1x32(fract32 _x, fract32 _y);

//32-bit mult with 40-bit buffer
extern fract32 mult_fr1x32x32(fract32 _x, fract32 _y);

//negation
extern fract32 negate_fr1x32(fract32 _x);

// abs
extern fract32 abs_fr1x32(fract32 _x);

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



// byte swap s32 value
extern s32 swap_s32 (s32 val);

///// print formatted string
extern void parameter_print(char* buf, u16 x);

// convert milliseconds to samples
#define MS_TO_SAMPS(x) ((x) * 48)

// convert data byte to hex characters
extern char* byte_to_hex_ascii(char* dst, u8 byte);

// convert hex characters to data byte
// return new position in char buffer
extern char* hex_ascii_to_byte(u8* dst, char* src);

// convert (4-byte) unsigned integer to hex ascii
// endian-specific!
void uint_to_hex_ascii(char* dst, u32 src);

// convert ascii hex string to uint
u32 hex_ascii_to_uint(char* str);

typedef struct {
    fract32 x;
    fract32 y;
    fract32 c;
} Slew32;

#define slew_init( sl, xa, ya, ca)			\
(sl.x) = (xa);					\
(sl.y) = (ya);					\
(sl.c) = (ca);

#define slew32_calc( sl )						\
(sl).y = add_fr1x32( (sl).x,						\
mult_fr1x32x32((sl).c,				\
sub_fr1x32((sl).y, (sl).x)	\
));

#define slew32_sync( sl ) ( sl.x == sl.y )

#endif // h guard
