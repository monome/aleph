#ifndef _ALEPH_MONOSYNTH_VOICE_H_
#define _ALEPH_MONOSYNTH_VOICE_H_
#include "env_adsr.h"
#include "ricks_tricks.h"
#include "filter_svf.h"

typedef struct _monosynth_voice {
  env_adsr filterEnv;
  filter_svf svf;

  u8 filtType;
  u8 clipFlavour;
  
  hpf dcBlocker;

  env_adsr ampEnv;
  
  fix16 noteHz;
  fix16 noteTune;
  phasor noteOsc;
  u8 oscFlavour;

  fract32 freqOn;
  fract32 freqOff;

} monosynthVoice;


extern void monosynthVoice_init(monosynthVoice* voice);
extern void monosynthVoice_noteon(monosynthVoice* voice);
extern void monosynthVoice_noteoff(monosynthVoice* voice);
extern fract32 monosynthVoice_next(monosynthVoice* voice);


#endif
