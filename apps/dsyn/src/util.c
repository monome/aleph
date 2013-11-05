#include "util.h"

static char hexCharLc[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
static char hexCharUc[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

// union of unsigned integer and 4-byte array
typedef union { 
  u32 ui;
  u8 b[4];
} uword_t ;

// convert data byte to 2 hex characters
// return incremented pointer to destination buf
char* byte_to_hex_ascii(char* dst, u8 byte) {
  *dst = hexCharLc[byte >> 4];
  dst++;
  *dst = hexCharLc[byte & 0xf]; 
  dst++;
  return dst;
}
// convert 2 hex characters to data byte
// return incremented pointer to source buf
char* hex_ascii_to_byte(u8* dst, char* src) {
  u8 i;
  for(i=0; i<16; i++) {
    if(*src == hexCharUc[i] || *src == hexCharLc[i]) {
      *dst = i << 4;
    }
  }
  src++;
  for(i=0; i<16; i++) {
    if( *src == hexCharUc[i] || *src == hexCharLc[i] ) {
      *dst |= i;
    }
  } 
  return ++src;
}

// convert unsigned integer to ascii hex string
void uint_to_hex_ascii(char* dst, u32 src) {
  uword_t x = { .ui = src };
  char* p = dst;
  // big-endian
  p = byte_to_hex_ascii(p, x.b[0]);
  p = byte_to_hex_ascii(p, x.b[1]);
  p = byte_to_hex_ascii(p, x.b[2]);
  p = byte_to_hex_ascii(p, x.b[3]);
}

// convert ascii hex string to uint
u32 hex_ascii_to_uint(char* str) {
  uword_t x = {.ui = 0x00000000 };
  char *p = str;
  // big-endian
  p = hex_ascii_to_byte(&(x.b[0]), p);
  p = hex_ascii_to_byte(&(x.b[1]), p);
  p = hex_ascii_to_byte(&(x.b[2]), p);
  p = hex_ascii_to_byte(&(x.b[3]), p);
  return x.ui;
}
