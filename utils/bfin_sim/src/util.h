#ifndef _ALEPH_BFIN_UTIL_
#define _ALEPH_BFIN_UTIL_

// some bitfield and bit-array macros
//#define BITS_PER_BYTE	8
#define BIT(x) (1 << (x))
#define BITMASK(b) (1 << ((b) % 8))
#define BITSLOT(b) ((b) >> 3)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITFLIP(a, b) ((a)[BITSLOT(b)] ^= BIT(b))
#define BITTEST(a, b) (((a)[BITSLOT(b)] & BITMASK(b)) > 0)
#define BITNSLOTS(nb) ((nb + BITS_PER_BYTE - 1) >> 3)

// and some byte-swapping functions, unfortunately!
void byte_swap(u8* pTarget, u8 bytes);

#endif // header guard
