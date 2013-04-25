/* buffer.c
   // audio lib
   // aleph

   audio buffer implementation.
  

 */


#ifdef ARCH_BFIN // bfin
#include "fract_math.h"
#include <fract2float_conv.h>
#else // linux
#include "fract32_emu.h"
#endif

#include "module.h"
#include "buffer.h"
//---------------------------
//--- static

// utility function to convert time in 16.16 to samples in 32.32
// FIXME; could abstract this somehow in fix32.h
static inline void time_to_samps(fix16* time, fix32* samps, u32 sr) {
  static fix16 fixTmp;
  // integer part
  samps->i = (s32) ( (u32)(FIX16_TO_U16(*time)) * (u32)sr );
  // multply fractional part
  fixTmp = fix16_mul(*time & 0xffff, sr << 16);
  // carry
  samps->i += FIX16_TO_U16(fixTmp);
  // remainder
  samps->fr = FIX16_FRACT_TRUNC(fixTmp);
}

//-----------------------
//---- extern

// initialize a (mono) audio buffer at pre-allocated memory
// give a separate contiguous chunk so we can use an arbitrary size
// but keep it statically linked at SDRAM
void buffer_init(audioBuffer *buf, fract32 *data, u32 frames,  u32 sr) {
  u32 i;
  buf->data = data;
  buf->sr = sr;
  buf->frames = frames;
  for(i=0; i<frames; i++) {
    buf->data[i] = 0;
  }  
}

// interpolated read
fract32 buffer_tap_read(bufferTap *tap) {
  static s32 idxB;
  static fract32 a, b;
  idxB = tap->idx.i + 1;
  while(idxB > tap->maxIdx) { idxB -= tap->buf->frames; }
  // we can assume idxA is already wrapped
  a = tap->buf->data[tap->idx.i];
  b = tap->buf->data[idxB];
  // apply interpolation from fractional index
  return add_fr1x32(a, mult_fr1x32x32(tap->idx.fr, sub_fr1x32(b, a)));
}

// interpolated write
void buffer_tap_write(bufferTap *tap, fract32 val) { 
  static s32 idxB;
  static fract32 a, b;  
  a = mult_fr1x32x32(val, sub_fr1x32(FR32_ONE, tap->idx.fr));
  b = mult_fr1x32x32(val, tap->idx.fr);
  idxB = tap->idx.i + 1;
  while(idxB > tap->maxIdx) { idxB -= tap->buf->frames; }
  // we can assume idxA is already wrapped
  tap->buf->data[tap->idx.i] = a;
  tap->buf->data[idxB] = b;  
}

// increment position of a tap
void buffer_tap_next(bufferTap *tap) {
  add_fix32(&(tap->idx), &(tap->inc));
  fix32_wrap_range(&(tap->idx), tap->buf->frames );
}

// set rate (per-sample increment)
void buffer_tap_set_rate(bufferTap *tap, fix16 rate) {
   fix16_to_fix32(&rate, &(tap->inc));
}

// intialize index data with buffer
void buffer_tap_init(bufferTap * tap, audioBuffer* buf) {
  tap->buf = buf;
  tap->maxIdx = buf->frames - 1;
  tap->inc.i = 0;
  tap->inc.fr = 0;
  tap->idx.i = 0;
  tap->idx.fr = 0;
}

// synchronize one tap with another at a given offset in seconds.
// very useful for delays
void buffer_tap_sync(bufferTap* tap, bufferTap* target, fix16 offset) {
  static fix32 sampsOff;
  time_to_samps(&offset, &sampsOff, tap->buf->sr);
  tap->idx = target->idx;
  sub_fix32(&(tap->idx), &sampsOff);
  fix32_wrap_range(&(tap->idx), tap->buf->frames);
}
