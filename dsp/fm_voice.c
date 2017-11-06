#include "fm_voice.h"
#include "fract_math.h"
#include "fix.h"
#include "libfixmath/fix16_fract.h"
#include "osc_polyblep.h"

void fm_voice_init (fm_voice *v, u8 nOps, u8 nModPoints) {
  v->nOps = nOps;
  v->nModPoints = nModPoints;
  v->noteHz = 440 << 16;
  v->noteTune = FIX16_ONE;
  v->portamento = SLEW_100MS;
  v->noteBaseFreq = 0;

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
    v->opFreqs[i] = 0;
    env_adsr_init(&(v->opEnv[i]));
    v->opModLast[i] = 0;
    v->bandLimit[i] = 1;
    v->freqSaturate[i] = 1;
    v->opSlew[i] = SLEW_100MS;
  }
  for(i=0; i < FM_MOD_POINTS_MAX; i++) {
    v->opModPointsExternal[i] = 0;
    v->opModPointsLast[i] = 0;
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

#define FM_OVERSAMPLE_BITS 2
#define FM_OVERSAMPLE (1 << FM_OVERSAMPLE_BITS)
/* #define FM_SMOOTH ((fract16) (FR16_MAX * ((4.0 * PI * FM_OVERSAMPLE) / (1.0 + 4.0 * PI * FM_OVERSAMPLE)))) */
#define FM_SMOOTH ((fract16) (FR16_MAX * 0.7))

void fm_voice_next (fm_voice *v) {
  int i, j;
  fract16 oversample_outs[FM_OPS_MAX][FM_OVERSAMPLE],
    oversample_envs[FM_OPS_MAX][FM_OVERSAMPLE],
    oversample_modPoints[FM_MOD_POINTS_MAX][FM_OVERSAMPLE];

  fract32 opFreqTarget;
  fract32 envLast, envNext;
  normalised_logSlew(&(v->noteBaseFreq), fix16_mul_fract(v->noteHz, v->noteTune), v->portamento);
  /* v->noteBaseFreq = fix16_mul_fract(v->noteHz, v->noteTune); */
  for(i=0; i < v->nOps; i++) {
    envLast = v->opEnv[i].envOut;
    envNext = env_adsr_next(&(v->opEnv[i]));
    opFreqTarget = shr_fr1x32(fix16_mul_fract(v->noteBaseFreq, v->opTune[i]),
			      FM_OVERSAMPLE_BITS);
    normalised_logSlew(&(v->opFreqs[i]), opFreqTarget, v->opSlew[i]);
    /* coarse_logSlew(&(v->opFreqs[i]), opFreqTarget, v->opSlew[i]); */
    /* v->opFreqs[i] = opFreqTarget; */
    fract32 envInc = shr_fr1x32(envNext - envLast,
				FM_OVERSAMPLE_BITS);
    for(j=0; j < FM_OVERSAMPLE; j++) {
      oversample_envs[i][j] = trunc_fr1x32(envLast);
      envLast = add_fr1x32(envLast, envInc);
    }
  }
  for(i=0; i < v->nModPoints; i++) {
    fract32 modPointsInc = shr_fr1x32(v->opModPointsExternal[i] - v->opModPointsLast[i],
				      FM_OVERSAMPLE_BITS);
    for(j=0; j < FM_OVERSAMPLE; j++) {
      oversample_modPoints[i][j] = trunc_fr1x32(v->opModPointsLast[i]);
      v->opModPointsLast[i] = add_fr1x32(v->opModPointsLast[i], modPointsInc);
    }
    v->opModPointsLast[i] = v->opModPointsExternal[i];
  }

  for(j=0; j < FM_OVERSAMPLE; j++) {
    fract16 nextOpOutputs[FM_OPS_MAX];
    for(i=0; i < v->nOps; i++) {

      // calculate modulation point & bandlimit to 20kHz or so...
      // add first modulation source
      fract16 opMod;
      if(v->opMod1Source[i] < v->nOps) {
	opMod = multr_fr1x16(v->opOutputsInternal[v->opMod1Source[i]],
			      v->opMod1Gain[i]);
      }
      else {
	opMod = multr_fr1x16(oversample_modPoints[v->opMod1Source[i] - v->nOps][j],
			     v->opMod1Gain[i]);
      }
      // add second modulation source
      if(v->opMod2Source[i] < v->nOps) {
	opMod = add_fr1x16(opMod,
			   multr_fr1x16(v->opOutputsInternal[v->opMod2Source[i]],
					v->opMod2Gain[i]));
      }
      else {
	opMod = add_fr1x16(opMod,
			   multr_fr1x16(oversample_modPoints[v->opMod2Source[i] - v->nOps][j],
					v->opMod2Gain[i]));
      }

      opMod = shr_fr1x32(opMod, 2);

      if(v->bandLimit[i]) {
	//bandlimit modulation signal with 20kHz iir
	opMod = mult_fr1x16(opMod, FR16_MAX - FM_SMOOTH);
	opMod = add_fr1x16(opMod,
			   multr_fr1x16(v->opModLast[i], FM_SMOOTH));
	v->opModLast[i] = opMod;
      }
      // phase increment each op with the oversample-compensated frequency,
      // calculate the op output for next oversampled frame
      fract32 opPhase = phasor_next_dynamic(&(v->opOsc[i]), v->opFreqs[i]);
      if(v->freqSaturate[i]) {
	opPhase += shl_fr1x32(opMod, 20);
      }
      else {
	opPhase += (opMod << 20);
      }
      fract16 oscSignal;
      oscSignal = sine_polyblep(opPhase);
      nextOpOutputs[i] = multr_fr1x16(oversample_envs[i][j],
				      oscSignal);
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
