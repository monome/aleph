#ifndef _UTIL_H_
#define _UTIL_H_

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

