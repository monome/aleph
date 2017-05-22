#include "monosynth_voice.h"

extern void monosynthVoice_init(monosynthVoice* voice) {
  env_adsr_init(&voice->ampEnv);
  env_adsr_init(&voice->filterEnv);
  filter_svf_init(&voice->svf);
  voice->phasor = 0;
  phasor_init (&voice->noteOsc);
  hpf_init(&voice->dcBlocker);
  voice->clipFlavour = 0;
  voice->oscFlavour = 0;
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
    note = voice->saw_polyblep(phase, freq);
    break;
  case 1 : // square polyblep
    note = voice->square_polyblep(phase, freq);
    break;
  case 2 : // triangle polyblep
    note = voice->triangle_polyblep(phase, freq);
    break;
  case 3 : // sine polyblep
    note = voice->sine_polyblep(phase, freq);
    break;
  default :
    note = voice->saw_polyblep(phase, freq);
  }
  note = shl_fr1x32(note, 15);
  note = mult_fr1x32x32(note, env_adsr_next(&voice->ampEnv));

  filter_svf_set_coeff(&voice->svf, adsr_next(&voice->filterEnv));
  
  switch (voice->clipFlavour) {
  case 0 : // asym clipping
    note = filter_svf_softclip_asym_lpf_next(&voice->svf, note);
    break;
  case 1 : // sym clipping
    note = filter_svf_softclip_lpf_next(&voice->svf, note);
    break;
  case 2 : // hard clipping only
    note = filter_svf_lpf_next(&voice->svf, note);
    break;
  default : // default to asym clipping
    note = filter_svf_softclip_asym_lpf_next(&voice->svf, note);
    break;
  }
  
  return dc_block(voice->dcBlocker, note);

}
