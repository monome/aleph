#ifndef FM_VOICE_H
#define FM_VOICE_H
#include "types.h"
#include "env_adsr.h"
#include "ricks_tricks.h"

#define FM_OPS_MAX 8

typedef struct fm_voice {
  u8 nOps;

  fract32 noteLevel;
  fix16 noteHz;
  fix16 noteTune;
  phasor lfo;
  u8 lfoWaveshape;

  fract32 opTune[FM_OPS_MAX];
  u8 opMod1Source[FM_OPS_MAX];
  fract32 opMod1Gain[FM_OPS_MAX];
  u8 opMod2Source[FM_OPS_MAX];
  fract32 opMod2Gain[FM_OPS_MAX];
  phasor opOsc[FM_OPS_MAX];
  env_adsr opEnv[FM_OPS_MAX];
  u8 opWaveshape[FM_OPS_MAX];

  fract32 opOutputs[FM_OPS_MAX];
} fm_voice;
void fm_voice_init (fm_voice *v, u8 nOps);
void fm_voice_press (fm_voice *v);
void fm_voice_release (fm_voice *v);
void fm_voice_next (fm_voice *v);

#endif
