#include "monosynth_voice.h"
#include "osc_polyblep.h"

extern void monosynthVoice_init(monosynthVoice* voice) {
  env_adsr_init(&voice->ampEnv);
  env_adsr_init(&voice->filterEnv);
  filter_svf_init(&voice->svf);
  phasor_init (&voice->noteOsc);
  hpf_init(&voice->dcBlocker);
  voice->clipFlavour = 0;
  voice->filtType = 0;
  voice->oscFlavour = 0;
  voice->freqOff = 0xfffff;
  voice->freqOn = 0x1fffffff;
}
extern void monosynthVoice_noteon(monosynthVoice* voice) {
  env_adsr_press(&voice->ampEnv);
  env_adsr_press(&voice->filterEnv);
}

extern void monosynthVoice_noteoff(monosynthVoice* voice) {
  env_adsr_release(&voice->ampEnv);
  env_adsr_release(&voice->filterEnv);
}

extern fract32 monosynthVoice_next(monosynthVoice* voice) {
  fract32 note;
  fract32 phase = phasor_next(&voice->noteOsc);
  fract32 freq = voice->noteOsc.freq;
  switch (voice->oscFlavour) {
  case 0 : // saw polyblep
    note = saw_polyblep(phase, freq);
    break;
  case 1 : // square polyblep
    note = square_polyblep(phase, freq);
    break;
  case 2 : // triangle polyblep
    note = triangle_polyblep(phase);
    break;
  case 3 : // sine polyblep
    note = sine_polyblep(phase);
    break;
  default :
    note = saw_polyblep(phase, freq);
  }
  note = shl_fr1x32(note, 15);
  note = mult_fr1x32x32(note, env_adsr_next(&voice->ampEnv));

  fract32 filtFreq;
  filtFreq = env_adsr_next(&(voice->filterEnv));
  filtFreq = mult_fr1x32x32(filtFreq, sub_fr1x32(voice->freqOn, voice->freqOff));
  filtFreq = add_fr1x32(filtFreq, voice->freqOff);
  filter_svf_set_coeff(&voice->svf, filtFreq);

  
  note = svf_funcs[voice->clipFlavour][voice->filtType](&voice->svf, note);
  
  return dc_block(&voice->dcBlocker, note);

}
