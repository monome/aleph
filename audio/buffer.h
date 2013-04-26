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
// data+discriptor class.
typedef struct _audioBuffer {
  // samplerate
  u32 sr;
  // count of frames
  u32 frames;
  // duration in seconds 
  fix16 dur;
  // pointer to data
  fract32 *data;
} audioBuffer;


// ---- bufferTap
// class for creating a "tap" or "head."
 // stores position/rate within a buffer.
typedef struct _bufferTap {
  // pointer to buf
  audioBuffer* buf;
  // max index
  u32 maxIdx;
  // current index
  fix32 idx;
  // phase increment 
  fix32 inc;
} bufferTap;

// initialize a (mono) audio buffer at pre-allocated memory.
// provide 2nd pointer for data
// so it can be placed in an arbitrarily separate memory region.
void buffer_init(audioBuffer* buf, fract32* data, u32 frames, u32 sr);

// intialize
void buffer_tap_init(bufferTap* tap, audioBuffer* buf);

// increment the index in a tsp
void buffer_tap_next(bufferTap* tap);


// interpolated read
fract32 buffer_tap_read(bufferTap* tap);

// interpolated write
// FIXME: better-considered behavior for multiple taps writing
void buffer_tap_write(bufferTap* tap, fract32 val); 

// set rate
void buffer_tap_set_rate(bufferTap *tap, fix16 rate);

// set a different buffer (resets position)
void buffer_tap_set_buf(bufferTap* tap, audioBuffer* buf);

// synchronize one tap with another at a given offset in seconds.
// very useful for delays
void buffer_tap_sync(bufferTap* tap, bufferTap* target, fix16 offset);
 
#endif // h guarduff
