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
    v->opOutputs[i] = 0;
    v->opOutputsInternal[i] = 0;
    phasor_init(&(v->opOsc[i]));
    env_adsr_16_init(&(v->opEnv[i]));
    v->opWaveshape[i] = 0;
    v->opModLast[i] = 0;
  }
}

void fm_voice_press (fm_voice *v) {
  int i;
  for(i=0; i < v->nOps; i++) {
    env_adsr_16_press(&(v->opEnv[i]));
  }
}
void fm_voice_release (fm_voice *v) {
  int i;
  for(i=0; i < v->nOps; i++) {
    env_adsr_16_release(&(v->opEnv[i]));
  }
}

#define FM_OVERSAMPLE_BITS 2
#define FM_OVERSAMPLE (1 << FM_OVERSAMPLE_BITS)
#define FM_SMOOTH ((fract16) (FR16_MAX * ((8.0 * PI * FM_OVERSAMPLE) / (1.0 + 8.0 * PI * FM_OVERSAMPLE))))

void fm_voice_next (fm_voice *v) {
  int i, j;
  fract16 oversample_outs[FM_OPS_MAX][FM_OVERSAMPLE];

  fract32 opFreqs[FM_OPS_MAX];
  fract16 envs[FM_OPS_MAX];
  fract32 baseFreq = fix16_mul_fract(v->noteHz, v->noteTune);
  for(i=0; i < v->nOps; i++) {
    envs[i] = env_adsr_16_next(&(v->opEnv[i]));
    opFreqs[i] = fix16_mul_fract(baseFreq, v->opTune[i]);
    opFreqs[i] = shr_fr1x32(opFreqs[i], FM_OVERSAMPLE_BITS);
  }

  for(j=0; j < FM_OVERSAMPLE; j++) {
    fract16 nextOpOutputs[FM_OPS_MAX];
    for(i=0; i < v->nOps; i++) {

      // calculate modulation point & bandlimit to 20kHz or so...
      // add first modulation source
      fract16 opMod = multr_fr1x16(v->opOutputsInternal[v->opMod1Source[i]],
				   v->opMod1Gain[i]);
      // add second modulation source
      opMod = add_fr1x16(opMod,
			 multr_fr1x16(v->opOutputs[v->opMod2Source[i]],
				      v->opMod2Gain[i]));

      //bandlimit modulation signal with 20kHz iir
      opMod = mult_fr1x16(opMod, FR16_MAX - FM_SMOOTH);
      opMod = add_fr1x16(opMod,
      			 multr_fr1x16(v->opModLast[i], FM_SMOOTH));
      v->opModLast[i] = opMod;

      // phase increment each op with the oversample-compensated frequency,
      // calculate the op output for next oversampled frame
      fract32 opPhase = phasor_next_dynamic(&(v->opOsc[i]), opFreqs[i]);
      nextOpOutputs[i] = multr_fr1x16(envs[i],
				      osc16(opPhase + shl_fr1x32(opMod, 22)));
    }

    for(i=0; i < v->nOps; i++) {
      // shuffle the op outputs into the buffer for next oversampled frame
      v->opOutputsInternal[i] = nextOpOutputs[i];
      // set up the oversampled output to sum & average
      oversample_outs[i][j] = shr_fr1x16(nextOpOutputs[i],
					 FM_OVERSAMPLE_BITS);
    }
  }

  // convert the oversampled output back to original sample rate
  for(i=0; i < v->nOps; i++) {
    v->opOutputs[i] = 0;
    for(j=0; j < FM_OVERSAMPLE; j++) {
      v->opOutputs[i] = add_fr1x16(oversample_outs[i][j],
				   v->opOutputs[i]);
    }
  }
}
