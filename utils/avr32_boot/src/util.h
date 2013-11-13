/*
  util.h

  stupid global helper functions.

  FIXME: name this more desciptively, or somehting.
  hex-string processing routines in particular should 
  live somewhere else.

 */

#ifndef _ALEPH_AVR32_APP_DSYN_UTIL_H_
#define _ALEPH_AVR32_APP_DSYN_UTIL_H_

#include "types.h"

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
