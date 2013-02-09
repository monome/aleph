#ifndef _NULLP_AUDIO_H_ 
#define _NULLP_AUDIO_H_ 

#include <portaudio.h>
#include "fract32_emu.h"

#define BLOCKSIZE (512)
#define SAMPLERATE (44100)
#define NUMCHANNELS (2)

#define FORMAT (paFloat32)

// to look like bfin
extern fract32 in0;
extern fract32 in1;
extern fract32 in2;
extern fract32 in3;

extern fract32 out0;
extern fract32 out1;
extern fract32 out2;
extern fract32 out3;

extern void init_audio(void);
extern void deinit_audio(void);

#endif // header guard
