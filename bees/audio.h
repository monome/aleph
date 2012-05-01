/* audio.h
 * bees
 * aleph
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <portaudio.h>
#include "../common/types.h"

//=============== defines
// audio processing block size
#define SBX_BLOCKSIZE   32
// audio processing sample rate
#define SBX_SAMPLERATE  48000
// blockrate = samplerate / blocksize (rounded)
#define SBX_BLOCKRATE 1500

s8 audio_load_module(const char* libstring);
s8 audio_init(void);
s8 audio_deinit(void);

#endif // header guard
