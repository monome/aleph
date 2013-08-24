#ifndef _NULLP_AUDIO_H_ 
#define _NULLP_AUDIO_H_ 

#include <portaudio.h>
#include "fract32_emu.h"

#define BLOCKSIZE (512)
#ifndef SAMPLERATE
  #define SAMPLERATE (44100)
#endif
#define NUMCHANNELS (2)

#define FORMAT (paFloat32)

// to look like bfin
extern fract32 in[4];

extern fract32 out[4];

extern void init_audio(void);
extern void deinit_audio(void);

#endif // header guard
