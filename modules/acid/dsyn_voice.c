#include "dsyn_voice.h"
#include "fract_math.h"

// initialize voice
void drumsyn_voice_init(drumsynVoice* voice) {
  // svf
  filter_svf_init(&(voice->svf));

  // noise
  acid_noise_init(&voice->noise);
		   
  // hipass
  filter_2p_hi_init(&(voice->hipass));

  // envelopes
  env_exp_init(&(voice->envAmp));
  env_exp_init(&(voice->envFreq));
  env_exp_init(&(voice->envRq));

  // SVF
  filter_svf_init(&(voice->svf));

  filter_svf_set_low(&(voice->svf), FR32_MAX >> 1);
  filter_svf_set_coeff(&(voice->svf), FR32_MAX >> 2);
  filter_svf_set_rq(&(voice->svf), FR32_MAX >> 3);

  env_exp_set_off( &(voice->envAmp) , 0 );
  env_exp_set_on(  &(voice->envAmp) , FR32_MAX >> 2 );
  
  env_exp_set_off(  &(voice->envFreq) , 0xffff );
  env_exp_set_on(  &(voice->envFreq) , 0x1fffffff );

  env_exp_set_off(  &(voice->envRq) , 0x1fffffff );
  env_exp_set_on(  &(voice->envRq) , 0x1fffffff );

  voice->freqEnv = 1;
  voice->rqEnv = 1;
  voice->svfPre = 1;

}

// next value of voice
fract32 drumsyn_voice_next(drumsynVoice* voice) {
  filter_svf* f = &(voice->svf);
  fract32 amp, freq, rq;

  amp = env_exp_next(&(voice->envAmp));

  if(voice->freqEnv > 0) {
    freq = env_exp_next(&(voice->envFreq));
    filter_svf_set_coeff(f, freq);
  }

  if(voice->rqEnv > 0) {
    rq = env_exp_next(&(voice->envRq));
    filter_svf_set_rq(f, rq);
  }

  if(voice->svfPre) {
    return shr_fr1x32(mult_fr1x32x32(amp,
				     filter_svf_next(f,
						     acid_noise_next(&voice->noise))),
		      1);
  } else {
    return shr_fr1x32(filter_svf_next(f,
				      mult_fr1x32x32(amp,
						     acid_noise_next(&voice->noise))),
		      1);
  }
}
