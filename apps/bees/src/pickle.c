/*
  pickle.c

  basically a little utility to enforce endianness across platforms.

 */


#include "print_funcs.h"

#include "types.h"
#include "pickle.h"
 
// 32-bit store to bytestream
// return updated stream pointer
u8* pickle_32(const u32 src, u8* dst) {
  *dst = src & 0xff;
  *(++dst) = (src & 0xff00) >> 8;
  *(++dst) = (src & 0xff0000) >> 16;
  *(++dst) = (src & 0xff000000) >> 24;
  return (++dst);
}

// 32-bit load from bytestream
// return updated stream pointer
const u8* unpickle_32(const u8* src, u32* dst) {
  *dst = 0;
  *dst |= *src;
  *dst |= (*(++src) << 8);
  *dst |= (*(++src) << 16);
  *dst |= (*(++src) << 24);

  return (++src);
}

// 16-bit
u8* pickle_16(const u16 src, u8* dst) {
  *dst = src & 0xff;
  *(++dst) = (src & 0xff00) >> 8;
  return (++dst);
}

const u8* unpickle_16(const u8* src, u16* dst) {
  *dst = 0;
  *dst |= *src;
  *dst |= *(++src) << 8;
  return (++src);
}

///// the idea is to keep this stuff the same on all architectures...
/* #else */
/* // other architectures... ?? */
/* #endif */




