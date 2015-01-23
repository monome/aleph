//util.h
//aleph-prgm-avr32

#ifndef _ALEPH_AVR32_APP_MIX_UTIL_H_
#define _ALEPH_AVR32_APP_MIX_UTIL_H_

#include "types.h"
#include <limits.h>

// byte swap s32 value
extern s32 swap_s32 (s32 val);

// convert float to fract32
//extern s32 float_to_fr32(float x);
extern fract32 float_to_fr32(f32 x);

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



#endif // h guard
