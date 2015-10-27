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

/*
void buffer_tap_set_loop(bufferTap *tap, u32 loop) {
  while(loop > (tap->buf->frames - 1)) {loop -= tap->buf->frames; }
  tap->loop = loop;
}
*/
// intialize index data with buffer
void buffer_tap_init(bufferTap * tap, audioBuffer* buf) {
  tap->buf = buf;
  tap->idx.i = 0;
  //tap->idx.fr = 0;

  //disabled for now - tap loop == buffer loop
  //tap->loop = buf->frames;

  //disabled for now until we have single speed taps working
  //tap->inc.i = 1;
  //tap->inc.fr = 0;
  //tap->idx.i = 0;
  //tap->idx.fr = 0;
}

// interpolated read
fract32 buffer_tap_read(bufferTap *tap) {
  //uninterpolated for now
  return tap->buf->data[tap->idx.i];


/*
  static s32 idxB;
  static fract32 a, b;
  idxB = tap->idx.i + 1;
  while(idxB >= tap->loop) { idxB -= tap->loop; }
  // we can assume idxA is already wrapped
  a = tap->buf->data[tap->idx.i];
  b = tap->buf->data[idxB];
  // apply interpolation from fractional index
  return add_fr1x32(a, mult_fr1x32x32(tap->idx.fr, sub_fr1x32(b, a)));
  */
}

// interpolated write (overwrites old contents)
void buffer_tap_write(bufferTap *tap, fract32 val) { 
  tap->buf->data[tap->idx.i] = val;
  /*
  static s32 idxB;
  static fract32 a, b;  
  a = mult_fr1x32x32(val, sub_fr1x32(FR32_MAX, tap->idx.fr));
  b = mult_fr1x32x32(val, tap->idx.fr);
  idxB = tap->idx.i + 1;
  while(idxB >= tap->loop) { idxB -= tap->loop; }
  // we can assume idxA is already wrapped
  tap->buf->data[tap->idx.i] = a;
  tap->buf->data[idxB] = b;  
  */
}

/*
// interpolated arbitrary mix of old buffer contents with new
void buffer_tap_mix(bufferTap *tap, fract32 val, fract32 preLevel) { 
  static s32 idxB;
static fract32 a, b;  
  a = mult_fr1x32x32(val, sub_fr1x32(FR32_MAX, tap->idx.fr));
  b = mult_fr1x32x32(val, tap->idx.fr);
  idxB = tap->idx.i + 1;
  while(idxB > tap->loop) { idxB -= tap->loop; }
  // we can assume idxA is already wrapped
  tap->buf->data[tap->idx.i] = add_fr1x32(a, mult_fr1x32x32(tap->buf->data[tap->idx.i], preLevel));
  tap->buf->data[tap->idx.i] = add_fr1x32(b, mult_fr1x32x32(tap->buf->data[idxB], preLevel));
  tap->buf->data[idxB] = b;  
}
*/


/*
// interpolated addition of input to buffer contents
void buffer_tap_add(bufferTap *tap, fract32 val) { 
   static s32 idxB; 
   static fract32 a, b;   
   a = mult_fr1x32x32(val, sub_fr1x32(FR32_MAX, tap->idx.fr)); 
   b = mult_fr1x32x32(val, tap->idx.fr); 
   idxB = tap->idx.i + 1; 
   while(idxB >= tap->loop) { idxB -= tap->loop; } 
   // we can assume idxA is already wrapped 
   tap->buf->data[tap->idx.i] = add_fr1x32(a, mult_fr1x32x32(tap->buf->data[tap->idx.i], preLevel)); 
   tap->buf->data[tap->idx.i] = add_fr1x32(b, mult_fr1x32x32(tap->buf->data[idxB], preLevel)); 
   tap->buf->data[idxB] = b;   
}*/


// increment position of a tap
void buffer_tap_next(bufferTap *tap) {
    //tap->idx.i += tap->inc.i;
    tap->idx.i ++;
    if (tap->idx.i >= tap->buf->frames) {
        tap->idx.i = 0;
    }
}


/*
// set rate (per-sample increment)
void buffer_tap_set_rate(bufferTap *tap, fix16 rate) {
   fix16_to_fix32(&rate, &(tap->inc));
}
*/

// set tap position directly
void buffer_tap_set_pos(bufferTap* tap, fix32 samples) {
  //fix32_wrap_range(&samples, tap->loop);
  tap->idx = samples;
}

// synchronize one tap with another at a given offset in seconds.
// useful for delays
void buffer_tap_sync(bufferTap* tap, bufferTap* target, fix32 samps) {
  tap->idx.i=target->idx.i-samps.i;
  return;
  fix32 samps_diff;
  if(target->idx.i >= samps.i) {
    // tap->idx = target->idx - samps;
    samps_diff.i = target->idx.i - samps.i ;
    //samps_diff.fr = 0;
  } else {
    //    tap->idx = (target->idx + tap->loop) - samps;
    samps_diff.i = (target->idx.i + tap->buf->frames) - samps.i;
    //samps_diff.fr = 0;
    buffer_tap_set_pos(tap, samps_diff);

  }
}

//---- non-interpolated

// read
fract32 buffer_tapN_read(bufferTapN *tap) {

  // hmm...
#if 0
  fract32 a, b;
  fix16 tmp;

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

//  mul and add old buffer contents to new
void buffer_tapN_mix(bufferTapN *tap, fract32 val, fract32 preLevel) { 
  tap->buf->data[tap->idx] = 
    add_fr1x32(  mult_fr1x32x32(tap->buf->data[tap->idx], preLevel), val );
}


// addition of input to buffer contents
void buffer_tapN_add(bufferTapN *tap, fract32 val) { 
    tap->buf->data[tap->idx] = add_fr1x32(  tap->buf->data[tap->idx], val );
}

// increment position of a tap
void buffer_tapN_next(bufferTapN *tap) {
  tap->divCount++;
  if(tap->divCount >= tap->div) {
    tap->divCount = 0;
    tap->idx += tap->inc;
    while(tap->idx >= tap->loop) {
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
  // reset divcount? ok 
  tap->divCount = 0;
}

// set rate divisor
void buffer_tapN_set_loop(bufferTapN *tap, u32 loop) {
  while(loop > (tap->buf->frames - 1)) {loop -= tap->buf->frames; }
  tap->loop = loop;
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
    // tap->idx = target->idx - samps;
    buffer_tapN_set_pos(tap, target->idx - samps );
  } else {
    //    tap->idx = (target->idx + tap->loop) - samps;
    buffer_tapN_set_pos(tap, (target->idx + tap->loop) - samps );
  }
}

// set tap position directly (wraps to loop)
void buffer_tapN_set_pos(bufferTapN* tap, u32 samps) {
  while(samps > tap->loop) {
    samps -= tap->loop;
  }
  tap->idx = samps;
}


// copy all params
void buffer_tapN_copy( bufferTapN* src, bufferTapN* dst ) {
  dst->loop = src->loop;
  dst->idx = src->idx;
  dst->inc = src->inc;
  dst->div = src->div;
  dst->divCount = src->divCount;
}



//// interpolated writes with two different tap positions...


/* // write (overwrites old contents) */
/* void buffer_tapN_write_fade(bufferTapN *tapA, bufferTapN *tapB, fract32 val, fract32 pos) {  */
/*   //  tap->buf->data[tap->idx] = val; */
  
/* } */

/* //  arbitrary mix of old buffer contents with new */
/* void buffer_tapN_mix_fade(bufferTapN *tapA, bufferTapN *tapB, fract32 val, fract32 preLevel, fract32 pos) {  */
/*   tap->buf->data[tap->idx] =  */
/*     add_fr1x32(  mult_fr1x32x32(tap->buf->data[tap->idx], preLevel), val ); */
/* } */


/* // interpolated addition of input to buffer contents */
/* void buffer_tapN_add_fade(bufferTapN *tapA, bufferTapN *tapB, fract32 val, fract32 pos) {  */
/*     tap->buf->data[tap->idx] = add_fr1x32(  tap->buf->data[tap->idx], val ); */
/* } */


extern void bufferTap24_8_init(bufferTap24_8* tap, audioBuffer* buf){
  tap->buf = buf;
  tap->idx = 0;
  tap->idx_last = 0;

  //disabled for now - tap loop == buffer loop
  tap->loop = buf->frames * 256;

  //One sample per frame should be +256 each time
  tap->inc = 256;
  //tap starts at 0
  tap->idx = 0;
}

extern void bufferTap24_8_next(bufferTap24_8* tap){
    tap->idx = (tap->inc + tap->idx + tap->loop) % tap->loop;
}

extern fract32 bufferTap24_8_read(bufferTap24_8* tap){
  u32 samp1_index = tap->idx;
  u32 samp2_index = (tap->idx + 256 + tap->loop) % tap->loop;
  fract32 samp1 = tap->buf->data[samp1_index / 256];
  fract32 samp2 = tap->buf->data[samp2_index / 256];
  fract32 inter_sample = FR32_MAX/256 * (tap->idx % 256);
  return pan_lin_mix(samp1, samp2, inter_sample) ;
}

extern void bufferTap24_8_set_rate(bufferTap24_8* tap, s32 inc) {
    tap->inc = inc;
}
extern fract32 bufferTap24_8_read_from(bufferTap24_8* tap, s32 idx){
  u32 samp1_index = idx;
  u32 samp2_index = (idx + 256 + tap->loop) % tap->loop;

  fract32 samp1 = tap->buf->data[samp1_index / 256];
  fract32 samp2 = tap->buf->data[samp2_index / 256];
  fract32 inter_sample = FR32_MAX/256 * (idx % 256);
  return pan_lin_mix(samp1, samp2, inter_sample) ;
}
/*
extern fract32 bufferTap24_8_read_antialias(bufferTap24_8* tap){
    //FIXME this should happen at the *end* of this subroutine.
    //Proably this isn't working after all, just taking an average of one
    //Doesn't work
    s32 idx = tap->idx;
    s32 idx_last = tap->idx_last;
    if (idx_last > idx)
        idx += tap->loop;
    s32 num_points = idx - idx_last / 256;
    if (num_points < 1)
        num_points = 1;
    fract32 weight = FR32_MAX / num_points;
    fract32 result = 0;
    while (num_points > 0) {
        result = add_fr1x32( result, mult_fr1x32x32( weight, bufferTap24_8_read_from(tap, idx) ) );
        idx -= 256;
        num_points --;
    }
    tap->idx_last = tap->idx;
    return result;

}
*/
extern void bufferTap24_8_set_loop(bufferTap24_8* tap, s32 loop){
    tap->loop = loop;
}

extern void bufferTap24_8_syncN(bufferTap24_8* tap, bufferTapN* target, s32 offset_subsamples) {
    tap->idx = ( (256 * target->idx - offset_subsamples ) + tap->loop ) % tap->loop;
}

extern void bufferTap24_8_set_pos(bufferTap24_8* tap, s32 idx) {
    tap->idx = idx;
}
/* //--------------------------- */
/* //---- crossfade */

/* // initialize crossfader */
/* void buffer_xfadeN_init(bufferXfadeN* fade, audioBuffer* buf) { */
/*   //... */
/* } */

/* // set new position, initiating fade */
/* void buffer_xfadeN_set_pos(bufferXfadeN* fade, u32 samps) { */
/*   //... */
/* } */

/* // get next value */
/* void buffer_xfadeN_next(bufferXfadeN* fade) { */
/*   // ... */
/* } */
