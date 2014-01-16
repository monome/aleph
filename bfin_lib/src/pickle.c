#include "types.h"
#include "pickle.h"

//#if ARCH_AVR32
// avr32 is big-endian
//// FIXME: should probably byteswap,
///  so that other systems can more easily parse the resultant blobs?
/// nah, doesn't really matter.
 
// 32-bit store to bytestream
// return updated stream pointer
u8* pickle_32(const u32 src, u8* dst) {
  // print_dbg("\r\n pickling word: 0x");
  // print_dbg_hex(src);
  *dst = src & 0xff;

  // print_dbg("\r\n pickle nibble: 0x");
  // print_dbg_hex(*dst);
  *(++dst) = (src & 0xff00) >> 8;

  // print_dbg("\r\n pickle nibble: 0x");
  // print_dbg_hex(*dst);
  *(++dst) = (src & 0xff0000) >> 16;

  // print_dbg("\r\n pickle nibble: 0x");
  // print_dbg_hex(*dst);
  *(++dst) = (src & 0xff000000) >> 24;

  // print_dbg("\r\n pickle nibble: 0x");
  // print_dbg_hex(*dst);
  return (++dst);
}

// 32-bit load from bytestream
// return updated stream pointer
const u8* unpickle_32(const u8* src, u32* dst) {
  *dst = 0;
  // print_dbg("\r\n unpickling src: 0x");
  // print_dbg_hex(*((u32*)src));
  
  *dst |= *src;
  // print_dbg("\r\n unpickled: 0x");
  // print_dbg_hex(*dst);

  *dst |= (*(++src) << 8);
  // print_dbg("\r\n unpickled: 0x");
  // print_dbg_hex(*dst);

  *dst |= (*(++src) << 16);
  // print_dbg("\r\n unpickled: 0x");
  // print_dbg_hex(*dst);

  *dst |= (*(++src) << 24);
  // print_dbg("\r\n unpickled: 0x");
  // print_dbg_hex(*dst);

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


