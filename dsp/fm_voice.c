#include "fm_voice.h"
#include "fract_math.h"
#include "fix.h"

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

void fm_voice_next (fm_voice *v) {
  int i;
  fract32 nextOpOutputs[FM_OPS_MAX];
  fract32 baseFreq = fix16_mul(v->noteHz, v->noteTune);
  for(i=0; i < v->nOps; i++) {
    fract32 opMod = fix16_mul(baseFreq, v->opTune[i]);
    opMod = add_fr1x32(opMod,
    		       mult_fr1x32x32(v->opOutputs[v->opMod1Source[i]],
    				      v->opMod1Gain[i]));
    opMod = add_fr1x32(opMod,
    		       mult_fr1x32x32(v->opOutputs[v->opMod2Source[i]],
    				      v->opMod2Gain[i]));
    fract32 opPhase = phasor_next_dynamic(&(v->opOsc[i]), opMod);
    nextOpOutputs[i] = mult_fr1x32x32(env_adsr_next(&(v->opEnv[i])), osc(opPhase));
  }
  for(i=0; i < v->nOps; i++) {
    v->opOutputs[i] = nextOpOutputs[i];
  }
  
}
