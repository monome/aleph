#include "dsyn_voice.h"
#include "fract_math.h"

// initialize voice
void drumsyn_voice_init(drumsynVoice* voice) {
  // svf
  filter_svf_init(&(voice->svf));

  // noise
  acid_noise_init(&voice->noise);
		   
  // envelopes
  env_trig_adsr_init(&(voice->envAmp));
  env_trig_adsr_init(&(voice->envFreq));

  // SVF
  filter_svf_init(&(voice->svf));

  filter_svf_set_low(&(voice->svf), FR32_MAX >> 1);
  filter_svf_set_coeff(&(voice->svf), FR32_MAX >> 2);
  filter_svf_set_rq(&(voice->svf), 0);

  voice->freqOff = 0xfffff;
  voice->freqOn = 0x1fffffff;

  voice->noiseGain = FR32_MAX >> 1;
  voice->postGain = FR32_MAX >> 1;

  voice->svfPre = 1;

}

void drumsyn_voice_bang(drumsynVoice* vp) {
  env_trig_adsr_bang(&(vp->envAmp));
  env_trig_adsr_bang(&(vp->envFreq));
}

// next value of voice
fract32 drumsyn_voice_next(drumsynVoice* voice) {
  filter_svf* f = &(voice->svf);
  fract32 amp, freq, rq;

  amp = env_trig_adsr_next(&(voice->envAmp));

  if(voice->freqEnv > 0) {
    freq = env_trig_adsr_next(&(voice->envFreq));
    freq = mult_fr1x32x32(freq, sub_fr1x32(voice->freqOn, voice->freqOff));
    freq = add_fr1x32(freq, voice->freqOn);
    filter_svf_set_coeff(f, freq);
  }

  fract32 n = mult_fr1x32x32(acid_noise_next(&voice->noise),
			     voice->noiseGain);
  if(voice->svfPre) {
    return mult_fr1x32x32(voice->postGain,
			  mult_fr1x32x32(amp, filter_svf_next(f, n)));
  } else {
    return mult_fr1x32x32(voice->postGain,
			  filter_svf_next(f, mult_fr1x32x32(amp,n)));
  }
}

