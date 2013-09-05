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
#include "stdio.h"
#endif

#include "fix.h"
#include "fix32.h"
#include "module.h"
#include "buffer.h"
#include "conversion.h"
//---------------------------
//--- static

//-----------------------
//---- extern

// initialize a (mono) audio buffer at pre-allocated memory
// give a separate contiguous chunk so we can use an arbitrary size
// but keep it statically linked at SDRAM
void buffer_init(audioBuffer *buf, fract32 *data, u32 frames) {
  //u32 i;
  buf->data = data;
  buf->frames = frames;
  /* for(i=0; i<frames; i++) { */
  /*   buf->data[i] = 0; */
  /* }   */
}

// interpolated read
fract32 buffer_tap_read(bufferTap *tap) {
  static s32 idxB;
  static fract32 a, b;
  idxB = tap->idx.i + 1;
  while(idxB >= tap->loop) { idxB -= tap->loop; }
  // we can assume idxA is already wrapped
  a = tap->buf->data[tap->idx.i];
  b = tap->buf->data[idxB];
  // apply interpolation from fractional index
  return add_fr1x32(a, mult_fr1x32x32(tap->idx.fr, sub_fr1x32(b, a)));
}

// interpolated write (overwrites old contents)
void buffer_tap_write(bufferTap *tap, fract32 val) { 
  static s32 idxB;
  static fract32 a, b;  
  a = mult_fr1x32x32(val, sub_fr1x32(FR32_MAX, tap->idx.fr));
  b = mult_fr1x32x32(val, tap->idx.fr);
  idxB = tap->idx.i + 1;
  while(idxB >= tap->loop) { idxB -= tap->buf->frames; }
  // we can assume idxA is already wrapped
  tap->buf->data[tap->idx.i] = a;
  tap->buf->data[idxB] = b;  
}

// interpolated arbitrary mix of old buffer contents with new
void buffer_tap_mix(bufferTap *tap, fract32 val, fract32 preLevel) { 
  static s32 idxB;
  static fract32 a, b;  
  a = mult_fr1x32x32(val, sub_fr1x32(FR32_MAX, tap->idx.fr));
  b = mult_fr1x32x32(val, tap->idx.fr);
  idxB = tap->idx.i + 1;
  while(idxB > tap->loop) { idxB -= tap->buf->frames; }
  // we can assume idxA is already wrapped
  tap->buf->data[tap->idx.i] = add_fr1x32(a, mult_fr1x32x32(tap->buf->data[tap->idx.i], preLevel));
  tap->buf->data[tap->idx.i] = add_fr1x32(b, mult_fr1x32x32(tap->buf->data[idxB], preLevel));
  tap->buf->data[idxB] = b;  
}


// interpolated addition of input to buffer contents
void buffer_tap_add(bufferTap *tap, fract32 val) { 
  /* static s32 idxB; */
  /* static fract32 a, b;   */
  /* a = mult_fr1x32x32(val, sub_fr1x32(FR32_MAX, tap->idx.fr)); */
  /* b = mult_fr1x32x32(val, tap->idx.fr); */
  /* idxB = tap->idx.i + 1; */
  /* while(idxB >= tap->loop) { idxB -= tap->loop; } */
  /* // we can assume idxA is already wrapped */
  /* tap->buf->data[tap->idx.i] = add_fr1x32(a, mult_fr1x32x32(tap->buf->data[tap->idx.i], preLevel)); */
  /* tap->buf->data[tap->idx.i] = add_fr1x32(b, mult_fr1x32x32(tap->buf->data[idxB], preLevel)); */
  /* tap->buf->data[idxB] = b;   */
}


// increment position of a tap
void buffer_tap_next(bufferTap *tap) {
  add_fix32(&(tap->idx), &(tap->inc));
  fix32_wrap_range(&(tap->idx), tap->loop );
}

// set rate (per-sample increment)
void buffer_tap_set_rate(bufferTap *tap, fix16 rate) {
   fix16_to_fix32(&rate, &(tap->inc));
}

// intialize index data with buffer
void buffer_tap_init(bufferTap * tap, audioBuffer* buf) {
  tap->buf = buf;
  tap->loop = buf->frames;
  tap->inc.i = 1;
  tap->inc.fr = 0;
  tap->idx.i = 0;
  tap->idx.fr = 0;
}

// set tap position directly
void buffer_tap_set_pos(bufferTap* tap, fix16 secs) {
  fix32 samps;
  sec_to_frames_fract(&secs, &samps);
  fix32_wrap_range(&samps, tap->loop);
  tap->idx = samps;
}

// synchronize one tap with another at a given offset in seconds.
// useful for delays
void buffer_tap_sync(bufferTap* tap, bufferTap* target, fix16 offset) {
  fix32 sampsOff;
  //  time_to_samps(&offset, &sampsOff, tap->buf->sr);
  sec_to_frames_fract(&offset, &sampsOff);
  tap->idx = target->idx;
  sub_fix32(&(tap->idx), &sampsOff);
  fix32_wrap_range(&(tap->idx), tap->loop);
}

//---- non-interpolated

// read
fract32 buffer_tapN_read(bufferTapN *tap) {
  fract32 a, b;
  fix16 tmp;

#if 1
  if(tap->divCount == 0) {
    return tap->buf->data[tap->idx];
  } else { // interpolate during phase-division
    a = tap->buf->data[tap->idx];
    if( (tap->idx + 1) >= tap->loop) {
      b = tap->buf->data[0];
    } else {
      b = tap->buf->data[ tap->idx + 1 ];
    }
    tmp = FRACT_FIX16( sub_fr1x32(b, a) );
    tmp = fix16_mul(tmp, fix16_from_int(tap->divCount));
    return add_fr1x32(a, FIX16_FRACT_TRUNC(tmp));
  }
#else
    return tap->buf->data[tap->idx];
#endif
}

// write (overwrites old contents)
void buffer_tapN_write(bufferTapN *tap, fract32 val) { 
  tap->buf->data[tap->idx] = val;
}

//  arbitrary mix of old buffer contents with new
void buffer_tapN_mix(bufferTapN *tap, fract32 val, fract32 preLevel) { 
  tap->buf->data[tap->idx] = 
    add_fr1x32(  mult_fr1x32x32(tap->buf->data[tap->idx], preLevel), val );
}


// interpolated addition of input to buffer contents
void buffer_tapN_add(bufferTapN *tap, fract32 val) { 
    tap->buf->data[tap->idx] = add_fr1x32(  tap->buf->data[tap->idx], val );
}

// increment position of a tap
void buffer_tapN_next(bufferTapN *tap) {
  tap->divCount++;
  if(tap->divCount == tap->div) {
    tap->divCount = 0;
    tap->idx += tap->inc;
    while(tap->idx > (tap->loop - 1)) {
      tap->idx -= tap->loop;
    }
  }
}

// set per-sample increment
void buffer_tapN_set_inc(bufferTapN *tap, u32 inc) {
  tap->inc = inc;
}

// set rate divisor
void buffer_tapN_set_div(bufferTapN *tap, u32 div) {
  tap->div = div;
}

// initialize tap with buffer descriptor
void buffer_tapN_init(bufferTapN * tap, audioBuffer* buf) {
  tap->buf = buf;
  tap->loop = buf->frames;
  tap->inc = 1;
  tap->div = 1;
  tap->idx = 0;
  tap->divCount = 0;
}

// synchronize one tap with another at a given offset in seconds.
// useful for delays
void buffer_tapN_sync(bufferTapN* tap, bufferTapN* target, u32 samps) {
  if(target->idx >= samps) {
    tap->idx = target->idx - samps;
  } else {
    tap->idx = (target->idx + tap->loop) - samps;
  }
}

// set tap position directly (wraps to loop)
void buffer_tapN_set_pos(bufferTapN* tap, u32 samps) {
  while(samps > tap->loop) {
    samps -= tap->loop;
  }
  tap->idx = samps;
}
