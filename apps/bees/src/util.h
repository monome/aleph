#ifndef _UTIL_H_
#define _UTIL_H_

#include "types.h"

// some bitfield and bit-array utilities
#define BITS_PER_BYTE	8
#define BIT(x) (1 << (x))
#define BITMASK(b) (1 << ((b) % BITS_PER_BYTE))
#define BITSLOT(b) ((b) / BITS_PER_BYTE)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITFLIP(a, b) ((a)[BITSLOT(b)] ^= BIT(b))
#define BITTEST(a, b) (((a)[BITSLOT(b)] & BITMASK(b)) > 0)
#define BITNSLOTS(nb) ((nb + BITS_PER_BYTE - 1) / BITS_PER_BYTE)



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



// append int to char buffer (left justified, no bounds)
/// very fast, for short unsigned values!
/*
inline char* atoi_idx(char* dst, u16 val) {
  u16 dig = 0;
  u16 rem = 0;
  char* pline = dst;
  // 3 digits only 
  if(val > 999) { rem = 999; } else { rem = val; }

  dig = val / 100U;
  rem -= dig * 100U;
  *pline = '0' + dig;
  ++pline;

  dig = rem / 10U;
  rem -= dig * 10U;
  *pline = '0' + dig;
  ++pline;

  *pline = '0' + rem;
  return  ++pline;    
}
*/

#endif // header guard

