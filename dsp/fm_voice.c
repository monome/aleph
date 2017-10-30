#include "fm_voice.h"
#include "fract_math.h"
#include "fix.h"
#include "libfixmath/fix16_fract.h"

void fm_voice_init (fm_voice *v, u8 nOps) {
  v->nOps = nOps;
  v->noteHz = 440 << 16;
  v->noteTune = FIX16_ONE;
  phasor_init(&(v->lfo));
  int i;
  for(i=0; i < nOps; i++) {
    v->opTune[i] = FIX16_ONE;
    v->opMod1Source[i] = i;
    v->opMod1Gain[i] = 0;
    v->opMod2Source[i] = (i - 1) % nOps;
    v->opMod2Gain[i] = 0;
    phasor_init(&(v->opOsc[i]));
    env_adsr_init(&(v->opEnv[i]));
    v->opWaveshape[i] = 0;
    v->opModLast[i] = 0;
  }
}

void fm_voice_press (fm_voice *v) {
  int i;
  for(i=0; i < v->nOps; i++) {
    env_adsr_press(&(v->opEnv[i]));
  }
}
void fm_voice_release (fm_voice *v) {
  int i;
  for(i=0; i < v->nOps; i++) {
    env_adsr_release(&(v->opEnv[i]));
  }
}

#define FM_OVERSAMPLE_BITS 1
#define FM_OVERSAMPLE (1 << FM_OVERSAMPLE_BITS)
#define FM_SMOOTH ((fract32) (FR32_MAX * ((4.0 * PI * FM_OVERSAMPLE) / (1.0 + 4.0 * PI * FM_OVERSAMPLE))))

void fm_voice_next (fm_voice *v) {
  int i;
#ifdef FM_OVERSAMPLE
  int j;
  fract32 oversample_outs[FM_OPS_MAX][FM_OVERSAMPLE];
  for(j=0; j < FM_OVERSAMPLE; j++) {
#endif
    fract32 nextOpOutputs[FM_OPS_MAX];
    fract32 baseFreq = fix16_mul_fract(v->noteHz, v->noteTune);
    for(i=0; i < v->nOps; i++) {
      fract32 opFreq = fix16_mul_fract(baseFreq, v->opTune[i]);
      fract32 opMod = mult_fr1x32x32(v->opOutputs[v->opMod1Source[i]],
				     v->opMod1Gain[i]);
      opMod = add_fr1x32(opMod,
			 mult_fr1x32x32(v->opOutputs[v->opMod2Source[i]],
					v->opMod2Gain[i]));
#ifdef FM_OVERSAMPLE
      opFreq  = shr_fr1x32(opFreq, FM_OVERSAMPLE_BITS);
      opMod = mult_fr1x32x32(opMod, FR32_MAX - FM_SMOOTH);
      opMod = add_fr1x32(opMod,
      			 mult_fr1x32x32(v->opModLast[i], FM_SMOOTH));
      v->opModLast[i] = opMod;
#endif
      fract32 opPhase = phasor_next_dynamic(&(v->opOsc[i]), opFreq);
      nextOpOutputs[i] = mult_fr1x32x32(env_adsr_next(&(v->opEnv[i])),
					osc(opPhase + opMod));
    }
    for(i=0; i < v->nOps; i++) {
      v->opOutputs[i] = nextOpOutputs[i];
#ifdef FM_OVERSAMPLE
      for(j=0; j < FM_OVERSAMPLE; j++) {
	oversample_outs[i][j] = shr_fr1x32(nextOpOutputs[i],
					   FM_OVERSAMPLE_BITS);
      }
#endif
    }
#ifdef FM_OVERSAMPLE
  }
  for(i=0; i < v->nOps; i++) {
    v->opOutputs[i] = 0;
    for(j=0; j < FM_OVERSAMPLE; j++) {
      v->opOutputs[i] = add_fr1x32(oversample_outs[i][j],
				   v->opOutputs[i]);
    }
  }
#endif
}
