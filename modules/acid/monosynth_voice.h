#ifndef _ALEPH_MONOSYNTH_VOICE_H_
#define _ALEPH_MONOSYNTH_VOICE_H_
#include "env_adsr.h"
#include "ricks_tricks.h"
#include "filter_svf.h"

typedef struct _monosynth_voice {
  env_adsr filterEnv;
  u8 clipFlavour;
  filter_svf svf;
  
  hpf dcBlocker;

  env_adsr ampEnv;
  
  fix16 noteHz;
  fix16 noteTune;
  phasor noteOsc;
  u8 oscFlavour;

} monosynthVoice;


extern void monosynthVoice_init(monosynthVoice* voice);
extern void monosynthVoice_noteon(monosynthVoice* voice);
extern void monosynthVoice_noteoff(monosynthVoice* voice);
extern fract32 monosynthVoice_next(monosynthVoice* voice);


#endif
