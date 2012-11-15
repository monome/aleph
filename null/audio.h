#ifndef _NULLP_AUDIO_H_ 
#define _NULLP_AUDIO_H_ 

#include <portaudio.h>

#define BLOCKSIZE (512)
#define SAMPLERATE (44100)
#define NUMCHANNELS (2)

#define FORMAT (paFloat32)

extern void init_audio(void);
extern void deinit_audio(void);

#endif // header guard
