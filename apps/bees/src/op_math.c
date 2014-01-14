#include "op_math.h"


// standard (overflow)
io_t op_add(io_t a, io_t b) { return ((a) + (b)); }
io_t op_sub(io_t a, io_t b) { return ((a) - (b)); }
io_t op_mul(io_t a, io_t b) { return ((a) * (b)); }
io_t op_div(io_t a, io_t b) { return ((a) / (b)); }

/// saturating add
io_t op_sadd(io_t a, io_t b) {
  // 16 bit saturation
///// FIXME! probably need inline ASM
// satadd.w 	Rd, Rx, Ry 
// mulsatwh.w 	Rd, Rx, Ry 
// etc
// for now, is a naive C implementation
  s32 res32 = (s32)a + (s32)b;
  if( res32 > 0x00007fff ) {
    return 0x7fff;
  } else {
    if( res32 < (s32)0xffff8000 ) {
      return (s16)0x8000;
    } else {
      return (s16)res32;
    }
  }
}

/// saturating subtract
io_t op_ssub(io_t a, io_t b) {
  // 16 bit saturation
///// FIXME! probably need inline ASM
// satadd.w 	Rd, Rx, Ry 
// mulsatwh.w 	Rd, Rx, Ry 
// etc
// for now, is a naive C implementation
  s32 res32 = (s32)a - (s32)b;
  if( res32 > 0x00007fff ) {
    return 0x7fff;
  } else {
    if( res32 < (s32)0xffff8000 ) {
      return (s16)0x8000;
    } else {
      return (s16)res32;
    }
  }
}

// print
void op_print(char* buf, io_t x) { itoa_whole((s32)(x), (buf), 6); }
