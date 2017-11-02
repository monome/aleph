/* buffer16.c
   // audio lib
   // aleph

   audio buffer16 implementation.
  

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
#include "buffer16.h"
#include "conversion.h"
#include "pan.h"
//---------------------------
//--- static

//-----------------------
//---- extern

//---- non-interpolated
void buffer16_init(audioBuffer16 *buf, fract16 *data, u32 frames) {
  //u32 i;
  buf->data = data;
  buf->frames = frames;
  /* for(i=0; i<frames; i++) { */
  /*   buf->data[i] = 0; */
  /* }   */
}

// initialize tap with buffer16 descriptor
void buffer16_tapN_init(buffer16TapN * tap, audioBuffer16* buf) {
  tap->buf = buf;
  tap->loop = buf->frames;
  tap->inc = 1;
  tap->div = 1;
  tap->idx = 0;
  tap->divCount = 0;
}

// read
fract16 buffer16_tapN_read(buffer16TapN *tap) {
  return tap->buf->data[tap->idx];
}

// write (overwrites old contents)
void buffer16_tapN_write(buffer16TapN *tap, fract16 val) { 
  tap->buf->data[tap->idx] = val;
}

//  mul and add old buffer16 contents to new
void buffer16_tapN_mix(buffer16TapN *tap, fract16 val, fract16 preLevel) { 
  tap->buf->data[tap->idx] = 
    add_fr1x32(  multr_fr1x16(tap->buf->data[tap->idx], preLevel), val );
}

// addition of input to buffer16 contents
void buffer16_tapN_add(buffer16TapN *tap, fract16 val) { 
    tap->buf->data[tap->idx] = add_fr1x32(  tap->buf->data[tap->idx], val );
}

// increment position of a tap
void buffer16_tapN_next(buffer16TapN *tap) {
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
void buffer16_tapN_set_inc(buffer16TapN *tap, u32 inc) {
  tap->inc = inc;
}

// set rate divisor
void buffer16_tapN_set_div(buffer16TapN *tap, u32 div) {
  tap->div = div;
  // reset divcount? ok 
  tap->divCount = 0;
}

// set rate divisor
void buffer16_tapN_set_loop(buffer16TapN *tap, u32 loop) {
  while(loop > (tap->buf->frames - 1)) {loop -= tap->buf->frames; }
  tap->loop = loop;
}

// synchronize one tap with another at a given offset in seconds.
// useful for delays
void buffer16_tapN_sync(buffer16TapN* tap, buffer16TapN* target, u32 samps) {
  if(target->idx >= samps) {
    // tap->idx = target->idx - samps;
    buffer16_tapN_set_pos(tap, target->idx - samps );
  } else {
    //    tap->idx = (target->idx + tap->loop) - samps;
    buffer16_tapN_set_pos(tap, (target->idx + tap->loop) - samps );
  }
}

// set tap position directly (wraps to loop)
void buffer16_tapN_set_pos(buffer16TapN* tap, u32 samps) {
  while(samps > tap->loop) {
    samps -= tap->loop;
  }
  tap->idx = samps;
}

// copy all params
void buffer16_tapN_copy( buffer16TapN* src, buffer16TapN* dst ) {
  dst->loop = src->loop;
  dst->idx = src->idx;
  dst->inc = src->inc;
  dst->div = src->div;
  dst->divCount = src->divCount;
}

extern void buffer16Tap24_8_init(buffer16Tap24_8* tap, audioBuffer16* buf){
  tap->buf = buf;
  tap->idx = 0;
  tap->idx_last = 0;

  //disabled for now - tap loop == buffer16 loop
  tap->loop = buf->frames;

  //One sample per frame should be +256 each time
  tap->inc = 256;
  //tap starts at 0
  tap->idx = 0;
}

void buffer16Tap24_8_next(buffer16Tap24_8* tap){
  tap->idx_last = tap->idx;
  tap->idx = (tap->inc + tap->idx + (tap->loop << 8)) % (tap->loop << 8);
}

void buffer16Tap24_8_set_inc(buffer16Tap24_8 *tap, u32 inc) {
  tap->inc = inc;
}
//FIXME stop using ricks_tricks
#include "ricks_tricks.h"
fract16 buffer16Tap24_8_read_bspline(buffer16Tap24_8* tap) {
  u32 buffSize = sizeof(fract16) * (u32)tap->loop;
  fract16 *samp1 = tap->buf->data + (tap->idx >> 8);
  fract16 *samp2 = __builtin_bfin_circptr((void *)samp1, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
  fract16 *samp3 = __builtin_bfin_circptr((void *)samp2, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
  fract16 *samp0 = __builtin_bfin_circptr((void *)samp1, -sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
  fract16 inter_sample = (FR16_MAX >> 8);
  inter_sample *= (tap->idx & 0xff);
  return interp_bspline_fract16(inter_sample, *samp0, *samp1, *samp2, *samp3);
}

fract16 buffer16Tap24_8_read(buffer16Tap24_8* tap){
  u32 buffSize = sizeof(fract16) * (u32)tap->loop;
  fract16 *samp1 = tap->buf->data + (tap->idx >> 8);
  fract16 *samp2 = __builtin_bfin_circptr((void *)samp1, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
  fract16 inter_sample = (FR16_MAX >> 8);
  inter_sample *= (tap->idx & 0xff);
  return pan_lin_mix16(*samp1, *samp2, inter_sample);
}

void buffer16Tap24_8_write(buffer16Tap24_8* tap, fract16 samp) {
  u32 writeIdx;
  fract16 *writePtr;
  fract16 inter_sample;
  u32 buffSize = sizeof(fract16) * (u32)tap->loop;
  if (tap->inc > 0) {
    writeIdx = (tap->idx_last >> 8);
    writePtr = tap->buf->data + writeIdx;
    if (tap->inc >= 256) {
      // fast interpolating write
      for (inter_sample = 0;
	   inter_sample < tap->inc - (tap->idx &0xff);
	   inter_sample += 256) {

	writeIdx++;
	fract16 pan = (writeIdx << 8) - tap->idx_last;
	pan *= FR16_MAX / tap->inc;
	fract16 panned = pan_lin_mix16(tap->samp_last, samp, pan);
	writePtr = __builtin_bfin_circptr((void *)writePtr, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	/* *writePtr = add_fr1x16(*writePtr, panned); */
	*writePtr = panned;
	/* *writePtr = samp; */
      }
    }
    else {
      // slow interpolating write
      if (((tap->idx) >> 8) != (tap->idx_last >> 8)) {
	fract16 pan = tap->idx & 0xff;
	pan *= (FR16_MAX / tap->inc);
	writePtr = __builtin_bfin_circptr((void *)writePtr, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
	fract16 panned = pan_lin_mix16(samp, tap->samp_last, pan);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	/* *writePtr = add_fr1x16(*writePtr, panned); */
	*writePtr = panned;
	/* *writePtr = samp; */
      }
    }
  }
  else if (tap->inc < 0) {
    writeIdx = (tap->idx_last >> 8);
    writePtr = tap->buf->data + writeIdx;

    // DEBUG sanity check
    /* *writePtr = samp; */

    if (tap->inc <= -256) {
      // fast interpolating write
      for (inter_sample = 1;
	   inter_sample > tap->inc + 256 - (tap->idx &0xff);
	   inter_sample -= 256) {

	fract16 pan = (writeIdx << 8) - tap->idx_last;
	pan *= FR16_MAX/tap->inc;
	fract16 panned = pan_lin_mix16(tap->samp_last, samp, pan);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	/* *writePtr = add_fr1x16(*writePtr, panned); */
	*writePtr = panned;
	/* *writePtr = pan; */
	/* *writePtr = samp; */
	writeIdx--;
	writePtr = __builtin_bfin_circptr((void *)writePtr, -sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
      }
    }
    else {
      // slow interpolating backward write
      if (((tap->idx) >> 8) != (tap->idx_last >> 8)) {
	fract16 pan = 256 - (tap->idx & 0xff);
	pan *= (FR16_MAX / (-tap->inc));
	fract16 panned = pan_lin_mix16(samp, tap->samp_last, pan);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	/* *writePtr = add_fr1x16(*writePtr, panned); */
	*writePtr = panned;
	/* *writePtr = samp; */
      }
    }
  }
  tap->samp_last = samp;
}

void buffer16Tap24_8_add(buffer16Tap24_8* tap, fract16 samp) {
  u32 writeIdx;
  fract16 *writePtr;
  fract16 inter_sample;
  u32 buffSize = sizeof(fract16) * (u32)tap->loop;
  if (tap->inc > 0) {
    writeIdx = (tap->idx_last >> 8);
    writePtr = tap->buf->data + writeIdx;
    if (tap->inc >= 256) {
      // fast interpolating write
      for (inter_sample = 0;
	   inter_sample < tap->inc - (tap->idx &0xff);
	   inter_sample += 256) {

	writeIdx++;
	fract16 pan = (writeIdx << 8) - tap->idx_last;
	pan *= FR16_MAX / tap->inc;
	fract16 panned = pan_lin_mix16(tap->samp_last, samp, pan);
	writePtr = __builtin_bfin_circptr((void *)writePtr, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = samp; */
      }
    }
    else {
      // slow interpolating write
      if (((tap->idx) >> 8) != (tap->idx_last >> 8)) {
	fract16 pan = tap->idx & 0xff;
	pan *= (FR16_MAX / tap->inc);
	writePtr = __builtin_bfin_circptr((void *)writePtr, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
	fract16 panned = pan_lin_mix16(samp, tap->samp_last, pan);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = samp; */
      }
    }
  }
  else if (tap->inc < 0) {
    writeIdx = (tap->idx_last >> 8);
    writePtr = tap->buf->data + writeIdx;

    // DEBUG sanity check
    /* *writePtr = samp; */

    if (tap->inc <= -256) {
      // fast interpolating write
      for (inter_sample = 1;
	   inter_sample > tap->inc + 256 - (tap->idx &0xff);
	   inter_sample -= 256) {

	fract16 pan = (writeIdx << 8) - tap->idx_last;
	pan *= FR16_MAX/tap->inc;
	fract16 panned = pan_lin_mix16(tap->samp_last, samp, pan);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = pan; */
	/* *writePtr = samp; */
	writeIdx--;
	writePtr = __builtin_bfin_circptr((void *)writePtr, -sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
      }
    }
    else {
      // slow interpolating backward write
      if (((tap->idx) >> 8) != (tap->idx_last >> 8)) {
	fract16 pan = 256 - (tap->idx & 0xff);
	pan *= (FR16_MAX / (-tap->inc));
	fract16 panned = pan_lin_mix16(samp, tap->samp_last, pan);
	/* *writePtr = multr_fr1x16(*writePtr, preLevel); */
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = samp; */
      }
    }
  }
  tap->samp_last = samp;
}

void buffer16Tap24_8_mix(buffer16Tap24_8* tap, fract16 samp, fract16 preLevel) {
  u32 writeIdx;
  fract16 *writePtr;
  fract16 inter_sample;
  u32 buffSize = sizeof(fract16) * (u32)tap->loop;
  if (tap->inc > 0) {
    writeIdx = (tap->idx_last >> 8);
    writePtr = tap->buf->data + writeIdx;
    if (tap->inc >= 256) {
      // fast interpolating write
      for (inter_sample = 0;
	   inter_sample < tap->inc - (tap->idx &0xff);
	   inter_sample += 256) {

	writeIdx++;
	fract16 pan = (writeIdx << 8) - tap->idx_last;
	pan *= FR16_MAX / tap->inc;
	fract16 panned = pan_lin_mix16(tap->samp_last, samp, pan);
	writePtr = __builtin_bfin_circptr((void *)writePtr, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
	*writePtr = multr_fr1x16(*writePtr, preLevel);
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = samp; */
      }
    }
    else {
      // slow interpolating write
      if (((tap->idx) >> 8) != (tap->idx_last >> 8)) {
	fract16 pan = tap->idx & 0xff;
	pan *= (FR16_MAX / tap->inc);
	writePtr = __builtin_bfin_circptr((void *)writePtr, sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
	fract16 panned = pan_lin_mix16(samp, tap->samp_last, pan);
	*writePtr = multr_fr1x16(*writePtr, preLevel);
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = samp; */
      }
    }
  }
  else if (tap->inc < 0) {
    writeIdx = (tap->idx_last >> 8);
    writePtr = tap->buf->data + writeIdx;

    // DEBUG sanity check
    /* *writePtr = samp; */

    if (tap->inc <= -256) {
      // fast interpolating write
      for (inter_sample = 1;
	   inter_sample > tap->inc + 256 - (tap->idx &0xff);
	   inter_sample -= 256) {

	fract16 pan = (writeIdx << 8) - tap->idx_last;
	pan *= FR16_MAX/tap->inc;
	fract16 panned = pan_lin_mix16(tap->samp_last, samp, pan);
	*writePtr = multr_fr1x16(*writePtr, preLevel);
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = pan; */
	/* *writePtr = samp; */
	writeIdx--;
	writePtr = __builtin_bfin_circptr((void *)writePtr, -sizeof(fract16),
					  (void *)tap->buf->data, buffSize);
      }
    }
    else {
      // slow interpolating backward write
      if (((tap->idx) >> 8) != (tap->idx_last >> 8)) {
	fract16 pan = 256 - (tap->idx & 0xff);
	pan *= (FR16_MAX / (-tap->inc));
	fract16 panned = pan_lin_mix16(samp, tap->samp_last, pan);
	*writePtr = multr_fr1x16(*writePtr, preLevel);
	*writePtr = add_fr1x16(*writePtr, panned);
	/* *writePtr = panned; */
	/* *writePtr = samp; */
      }
    }
  }
  tap->samp_last = samp;
}

void buffer16Tap24_8_set_rate(buffer16Tap24_8* tap, s32 inc) {
    tap->inc = inc;
}
fract16 buffer16Tap24_8_read_from(buffer16Tap24_8* tap, s32 idx){
  u32 samp1_index = idx;
  u32 samp2_index = (idx + 256 + tap->loop) % tap->loop;

  fract16 samp1 = tap->buf->data[samp1_index / 256];
  fract16 samp2 = tap->buf->data[samp2_index / 256];
  fract16 inter_sample = FR32_MAX/256 * (idx % 256);
  return pan_lin_mix16(samp1, samp2, inter_sample) ;
}

void buffer16Tap24_8_set_loop(buffer16Tap24_8* tap, s32 samples){
    tap->loop = samples;
}

void buffer16Tap24_8_syncN(buffer16Tap24_8* tap, buffer16TapN* target, s32 offset_subsamples) {
  tap->idx = ( (256 * target->idx - offset_subsamples ) + (tap->loop << 8) ) % (tap->loop << 8);
}

void buffer16Tap24_8_sync(buffer16Tap24_8* tap, buffer16Tap24_8* target, s32 offset_subsamples) {
  tap->idx = ( (target->idx - offset_subsamples ) + (tap->loop << 8) ) % (tap->loop << 8);
}

void buffer16Tap24_8_set_pos(buffer16Tap24_8* tap, s32 idx) {
    tap->idx = idx;
}

void buffer16Tap24_8_copy( buffer16Tap24_8* src, buffer16Tap24_8* dst ) {
  dst->idx_last = src->idx_last;
  dst->samp_last = src->samp_last;
  dst->idx = src->idx;
  dst->inc = src->inc;
  dst->loop = src->loop;
}
