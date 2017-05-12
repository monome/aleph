/* buffer16.h
   audio lib
   aleph

   classes for manipulating audio buffer16s.

*/

#ifndef _ALEPH_BUFFER16_H_
#define _ALEPH_BUFFER16_H_

#include "fix.h"
#include "fix32.h"
#include "types.h"

//--- audioBuffer16
// data+descriptor class.
typedef struct _audioBuffer16 {
  // count of frames
  u32 frames;
  // pointer to data
  volatile fract16 *data;
} audioBuffer16;


// ---- buffer16Tap
// class for creating a "tap" or "head."
 // stores position within a buffer16.
// supports interpolated read/write.
typedef struct _buffer16Tap {
  // pointer to buf
  audioBuffer16* buf;
  // current index
  fix32 idx;
} buffer16Tap;


// initialize a (mono) audio buffer16 at pre-allocated memory.
// provide 2nd pointer for data,
// so it can be placed in an arbitrarily separate memory region.
extern void buffer16_init(audioBuffer16* buf, fract16* data, u32 frames);

// intialize tap
extern void buffer16_tap_init(buffer16Tap* tap, audioBuffer16* buf);

// increment the index in a tap
extern void buffer16_tap_next(buffer16Tap* tap);

// interpolated read
extern fract16 buffer16_tap_read(buffer16Tap* tap);

// interpolated write (erases old contents)
extern void buffer16_tap_write(buffer16Tap* tap, fract16 val); 

// interpolated mix (old + new, arbitrary)
extern void buffer16_tap_mix(buffer16Tap* tap, fract16 val, fract16 preLevel); 

// interpolated add (new + old (unchanged)
extern void buffer16_tap_add(buffer16Tap* tap, fract16 val);

// set rate
extern void buffer16_tap_set_rate(buffer16Tap *tap, fix16 rate);

// set a different buffer16 (resets position)
extern void buffer16_tap_set_buf(buffer16Tap* tap, audioBuffer16* buf);

// set loop endpoint in seconds
extern void buffer16_tap_set_loop(buffer16Tap* tap, u32 loop);

// synchronize one tap with another at a given offset in seconds.
// useful for delays
extern void buffer16_tap_sync(buffer16Tap* tap, buffer16Tap* target, fix32 samples);
 
// set tap position directly
extern void buffer16_tap_set_pos(buffer16Tap* tap, fix32 samples);

//--------------------------------------------------------
//---------------------------------------------------------
//---- non-interpolated taps

// ---- buffer16TapN
// class for creating a "tap" or "head."
 // stores position/rate within a buffer16.
// non-interpolated.
typedef struct _buffer16TapN {
  // pointer to buf
  audioBuffer16* buf;
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
} buffer16TapN;

//---- buffer16XfadeN
// class for cross-fading between two non-interpolated read taps in a buffer16
typedef struct _buffer16XfadeN {
  // two read tapstap
  buffer16TapN read[2];
  // fade status
  /// 0 = tap 0 only
  /// 1 = tap 1 only
  /// 2 = both taps
  u8 satus;
  // fade levels
  u16 fadeLevel[2];
  // fade positions
  u16 fadePos[2];
} buffer16XfadeN;


//------------------------------------
//----- external functions

// intialize
extern void buffer16_tapN_init(buffer16TapN* tap, audioBuffer16* buf);

// increment the index in a tap 
extern void buffer16_tapN_next(buffer16TapN* tap);


// non-interpolated read
extern fract16 buffer16_tapN_read(buffer16TapN* tap);

// non-interpolated write (erases old contents)
extern void buffer16_tapN_write(buffer16TapN* tap, fract16 val);

// non-interpolated mix (old + new, arbitrary)
extern void buffer16_tapN_mix(buffer16TapN* tap, fract16 val, fract16 preLevel); 

// non-interpolated add (new + old (unchanged)
extern void buffer16_tapN_add(buffer16TapN* tap, fract16 val);

// set rate
//void buffer16_tapN_set_rate(buffer16TapN *tap, fix16 rate);

// set per-sample increment
extern void buffer16_tapN_set_inc(buffer16TapN *tap, u32 inc);

// set rate divisor
extern void buffer16_tapN_set_div(buffer16TapN *tap, u32 div);

// set a different buffer16 (resets position)
extern void buffer16_tapN_set_buf(buffer16TapN* tap, audioBuffer16* buf);

// set loop endpoint in samples
extern void buffer16_tapN_set_loop(buffer16TapN* tap, u32 samps);

// synchronize one tap with another at a given offset in seconds.
// useful for delays
extern void buffer16_tapN_sync(buffer16TapN* tap, buffer16TapN* target, u32 samps);

// set tap position directly
extern void buffer16_tapN_set_pos(buffer16TapN* tap, u32 samp);

// copy all params
extern void buffer16_tapN_copy( buffer16TapN* src, buffer16TapN* dst );


// ---- buffer16Tap24_8
// class for creating a "tap" or "head."
 // stores position/rate within a buffer16.
// supports interpolated read/write.
typedef struct _buffer16Tap24_8 {
  // pointer to buf
  audioBuffer16* buf;
  // last index
  s32 idx_last;
  s16 samp_last;
  // current index
  s32 idx;
  // phase increment
  s32 inc;
  // loop position
  s32 loop;
} buffer16Tap24_8;


// intialize tap
extern void buffer16Tap24_8_init(buffer16Tap24_8* tap, audioBuffer16* buf);

// increment the index in a tap
extern void buffer16Tap24_8_next(buffer16Tap24_8* tap);

// set speed
extern void buffer16Tap24_8_set_inc(buffer16Tap24_8 *tap, u32 inc);

// interpolating write
extern void buffer16Tap24_8_write(buffer16Tap24_8* tap, fract16 val);
// interpolating write mixing old signal
void buffer16Tap24_8_mix(buffer16Tap24_8* tap, fract16 samp, fract16 preLevel);
// interpoilating write adding old signal
void buffer16Tap24_8_add(buffer16Tap24_8* tap, fract16 samp);
// interpolated read
extern fract16 buffer16Tap24_8_read(buffer16Tap24_8* tap);

// interpolated read from arbitrary position
extern fract16 buffer16Tap24_8_read_from(buffer16Tap24_8* tap, s32 idx);

// set rate
extern void buffer16Tap24_8_set_rate(buffer16Tap24_8 *tap, s32 inc);

// set a different buffer16 (resets position)
//extern void buffer1624_8_tap_set_buf(Buffer16Tap24_8* tap, audioBuffer16* buf);

// set loop point in subsamples
extern void buffer16Tap24_8_set_loop(buffer16Tap24_8* tap, s32 loop);

// synchronize 24.8 interp tap with an non-interpolated tap at a given offset in subsamples.
extern void buffer16Tap24_8_syncN(buffer16Tap24_8* tap, buffer16TapN* target, s32 offset);
extern void buffer16Tap24_8_sync(buffer16Tap24_8* tap, buffer16Tap24_8* target, s32 offset_subsamples);

// set 24.8 interp tap position directly in subsamples
extern void buffer16Tap24_8_set_pos(buffer16Tap24_8* tap, s32 idx);

//---------------------------
//---- crossfade

// initialize crossfader
extern void buffer16_xfadeN_init(buffer16XfadeN* fade, audioBuffer16* buf);
// set new position, initiating fade
extern void buffer16_xfadeN_set_pos(buffer16XfadeN* fade, u32 samps);
// get next value
extern void buffer16_xfadeN_next(buffer16XfadeN* fade);

extern void buffer16Tap24_8_copy( buffer16Tap24_8* src, buffer16Tap24_8* dst );
#endif // h guard
