/* wire.c
 * nullp
 * 
 * stereo pass-through test module
 */

#include "audio.h"
#include "module.h"
#include "fract32.h"

void module_callback(const f32* in, f32* out) {
  u32 frame;
  u8 chan;
  
  for(frame=0; frame<BLOCKSIZE; frame++) {
    for(chan=0; chan<NUMCHANNELS; chan++) { // stereo interleaved
      *out++ = *in++;
    }
  }
}
