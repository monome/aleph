#ifndef FM_VOICE_H
#define FM_VOICE_H
#include "types.h"
#include "env_adsr.h"
#include "ricks_tricks.h"

#define FM_OPS_MAX 8
#define FM_MOD_POINTS_MAX 8

typedef struct fm_voice {
  u8 nOps, nModPoints;

  fix16 noteHz;
  fix16 noteTune;
  fract32 portamento;
  fract32 noteBaseFreq;

  fract32 opTune[FM_OPS_MAX];
  u8 opMod1Source[FM_OPS_MAX];
  fract16 opMod1Gain[FM_OPS_MAX];
  u8 opMod2Source[FM_OPS_MAX];
  fract16 opMod2Gain[FM_OPS_MAX];
  fract16 opModLast[FM_OPS_MAX];
  phasor opOsc[FM_OPS_MAX];
  fract32 opFreqs[FM_OPS_MAX];
  fract32 opSlew[FM_OPS_MAX];
  env_adsr opEnv[FM_OPS_MAX];
  u8 bandLimit[FM_OPS_MAX];
  u8 freqSaturate[FM_OPS_MAX];

  fract16 opOutputsInternal[FM_OPS_MAX];
  fract16 opOutputs[FM_OPS_MAX];
  fract32 opModPointsExternal[FM_MOD_POINTS_MAX];
  fract32 opModPointsLast[FM_MOD_POINTS_MAX];
} fm_voice;
void fm_voice_init (fm_voice *v, u8 nOps, u8 nModPoints);
void fm_voice_press (fm_voice *v);
void fm_voice_release (fm_voice *v);
void fm_voice_next (fm_voice *v);

#endif
