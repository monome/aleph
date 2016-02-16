/*
  pickle.h

  utilities for object serialization.
  (force endianness)

 */

#ifndef _ALEPH_BEES_PICKLE_H_
#define _ALEPH_BEES_PICKLE_H_

#include "types.h"

// 32-bit
extern u8* pickle_32(const u32 src, u8* dst);
extern const u8* unpickle_32(const u8* src, u32* dst);

// 16-bit
// ... probably never a great idea;
/// seems more convenient to maintain 4-byte alignment.
extern u8* pickle_16(const u16 src, u8* dst);
extern const u8* unpickle_16(const u8* src, u16* dst);


#endif
