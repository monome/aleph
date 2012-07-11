/* audio.h
 * bees
 * aleph
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <portaudio.h>
#include "types.h"

s8 audio_load_module(const char* libstring);
s8 audio_init(void);
s8 audio_deinit(void);

#endif // header guard
