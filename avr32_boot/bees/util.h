#ifndef _UTIL_H_
#define _UTIL_H_

// TODO: fract/float conversion

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

#endif // header guard
