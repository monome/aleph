/* buffer.h
   audio lib
   aleph

   classes for manipulating audio buffers.

*/

#ifndef _ALEPH_BUFFER_H_
#define _ALEPH_BUFFER_H_

#include "fix.h"
#include "fix32.h"
#include "types.h"

//--- audioBuffer
// data+descriptor class.
typedef struct _audioBuffer {
  // count of frames
  u32 frames;
  // pointer to data
  volatile fract32 *data;
} audioBuffer;


// ---- bufferTap
// class for creating a "tap" or "head."
 // stores position/rate within a buffer.
// supports interpolated read/write.
typedef struct _bufferTap {
  // pointer to buf
  audioBuffer* buf;
  // current index
  fix32 idx;
  // phase increment
  // disabled for now until we have single speed taps working
  //fix32 inc;
} bufferTap;


// initialize a (mono) audio buffer at pre-allocated memory.
// provide 2nd pointer for data,
// so it can be placed in an arbitrarily separate memory region.
extern void buffer_init(audioBuffer* buf, fract32* data, u32 frames);

// intialize tap
extern void buffer_tap_init(bufferTap* tap, audioBuffer* buf);

// increment the index in a tap
extern void buffer_tap_next(bufferTap* tap);

// interpolated read
extern fract32 buffer_tap_read(bufferTap* tap);

// interpolated write (erases old contents)
extern void buffer_tap_write(bufferTap* tap, fract32 val); 

// interpolated mix (old + new, arbitrary)
extern void buffer_tap_mix(bufferTap* tap, fract32 val, fract32 preLevel); 

// interpolated add (new + old (unchanged)
extern void buffer_tap_add(bufferTap* tap, fract32 val);

// set rate
extern void buffer_tap_set_rate(bufferTap *tap, fix16 rate);

// set a different buffer (resets position)
extern void buffer_tap_set_buf(bufferTap* tap, audioBuffer* buf);

// set loop endpoint in seconds
extern void buffer_tap_set_loop(bufferTap* tap, u32 loop);

// synchronize one tap with another at a given offset in seconds.
// useful for delays
extern void buffer_tap_sync(bufferTap* tap, bufferTap* target, fix32 samples);
 
// set tap position directly
extern void buffer_tap_set_pos(bufferTap* tap, fix32 samples);

//--------------------------------------------------------
//---------------------------------------------------------
//---- non-interpolated taps

// ---- bufferTapN
// class for creating a "tap" or "head."
 // stores position/rate within a buffer.
// non-interpolated.
typedef struct _bufferTapN {
  // pointer to buf
  audioBuffer* buf;
  // index to loop at (upper bound)
  u32 loop;
  // current index
  u32 idx;
  // phase increment
  u32 inc;
  // rate divisor
  u32 div;
  // current divisor count
  u32 divCount;
} bufferTapN;

//---- bufferXfadeN
// class for cross-fading between two non-interpolated read taps in a buffer
typedef struct _bufferXfadeN {
  // two read tapstap
  bufferTapN read[2];
  // fade status
  /// 0 = tap 0 only
  /// 1 = tap 1 only
  /// 2 = both taps
  u8 satus;
  // fade levels
  u16 fadeLevel[2];
  // fade positions
  u16 fadePos[2];
} bufferXfadeN;


//------------------------------------
//----- external functions

// intialize
extern void buffer_tapN_init(bufferTapN* tap, audioBuffer* buf);

// increment the index in a tap 
extern void buffer_tapN_next(bufferTapN* tap);


// non-interpolated read
extern fract32 buffer_tapN_read(bufferTapN* tap);

// non-interpolated write (erases old contents)
extern void buffer_tapN_write(bufferTapN* tap, fract32 val); 

// non-interpolated mix (old + new, arbitrary)
extern void buffer_tapN_mix(bufferTapN* tap, fract32 val, fract32 preLevel); 

// non-interpolated add (new + old (unchanged)
extern void buffer_tapN_add(bufferTapN* tap, fract32 val);

// set rate
//void buffer_tapN_set_rate(bufferTapN *tap, fix16 rate);

// set per-sample increment
extern void buffer_tapN_set_inc(bufferTapN *tap, u32 inc);

// set rate divisor
extern void buffer_tapN_set_div(bufferTapN *tap, u32 div);

// set a different buffer (resets position)
extern void buffer_tapN_set_buf(bufferTapN* tap, audioBuffer* buf);

// set loop endpoint in samples
extern void buffer_tapN_set_loop(bufferTapN* tap, u32 samps);

// synchronize one tap with another at a given offset in seconds.
// useful for delays
extern void buffer_tapN_sync(bufferTapN* tap, bufferTapN* target, u32 samps);

// set tap position directly
extern void buffer_tapN_set_pos(bufferTapN* tap, u32 samp);

// copy all params
extern void buffer_tapN_copy( bufferTapN* src, bufferTapN* dst );


// ---- bufferTap24_8
// class for creating a "tap" or "head."
 // stores position/rate within a buffer.
// supports interpolated read/write.
typedef struct _bufferTap24_8 {
  // pointer to buf
  audioBuffer* buf;
  // last index
  s32 idx_last;
  // current index
  s32 idx;
  // phase increment
  s32 inc;

  // loop position
  s32 loop;
} bufferTap24_8;


// intialize tap
extern void bufferTap24_8_init(bufferTap24_8* tap, audioBuffer* buf);

// increment the index in a tap
extern void bufferTap24_8_next(bufferTap24_8* tap);

// interpolated read
extern fract32 bufferTap24_8_read(bufferTap24_8* tap);

// interpolated read from arbitrary position
extern fract32 bufferTap24_8_read_from(bufferTap24_8* tap, s32 idx);

// antialiased interpolated read BROKEN
//extern fract32 bufferTap24_8_read_antialias(bufferTap24_8* tap);

// interpolated write (erases old contents)
//extern void buffer24_8_tap_write(BufferTap24_8* tap, fract32 val);

// interpolated mix (old + new, arbitrary)
//extern void buffer24_8_tap_mix(BufferTap24_8* tap, fract32 val, fract32 preLevel);

// interpolated add (new + old (unchanged)
//extern void buffer24_8_tap_add(BufferTap24_8* tap, fract32 val);

// set rate
extern void bufferTap24_8_set_rate(bufferTap24_8 *tap, s32 inc);

// set a different buffer (resets position)
//extern void buffer24_8_tap_set_buf(BufferTap24_8* tap, audioBuffer* buf);

// set loop point in subsamples
extern void bufferTap24_8_set_loop(bufferTap24_8* tap, s32 loop);

// synchronize 24.8 interp tap with an non-interpolated tap at a given offset in subsamples.
extern void bufferTap24_8_syncN(bufferTap24_8* tap, bufferTapN* target, s32 offset);

// set 24.8 interp tap position directly in subsamples
extern void bufferTap24_8_set_pos(bufferTap24_8* tap, s32 idx);

//---------------------------
//---- crossfade

// initialize crossfader
extern void buffer_xfadeN_init(bufferXfadeN* fade, audioBuffer* buf);
// set new position, initiating fade
extern void buffer_xfadeN_set_pos(bufferXfadeN* fade, u32 samps);
// get next value
extern void buffer_xfadeN_next(bufferXfadeN* fade);

#endif // h guard
