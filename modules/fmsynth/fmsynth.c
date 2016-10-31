/* fmsynth.c
   aleph-bfin

   (Monophonic) FM Synth.
*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"

// aleph-bfin
#include "bfin_core.h"
#include "cv.h"
#include "gpio.h"
#include "fract_math.h"

// audio
#include "filter_1p.h"
#include "filter_svf.h"
#include "biquad.h"
#include "ricks_tricks.h"
#include "osc_polyblep.h"
#include "env_adsr.h"
#include "fm_voice.h"

#include "module.h"

/// custom
#include "params.h"
#define PAN_DEFAULT PAN_MAX/2
#define FADER_DEFAULT PARAM_AMP_0

// data structure of external memory
typedef struct _fmsynthData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} fmsynthData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
fmsynthData* pDacsData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

fm_voice voice;
fract32 opVols[4];
fract32 opPans[4];

/* fract32 noteLevel; */
/* fix16 noteHz; */
/* fix16 noteTune; */

/* u8 opMod1Sources[4]; */
/* fract32 opMod1Gains[4]; */
/* u8 opMod2Sources[4]; */
/* fract32 opMod2Gains[4]; */
/* fix16 opTunes[4]; */
/* phasor opOscs[4]; */
/* env_adsr opEnvs[4]; */
/* u8 opWaveshapes[4]; */
/* phasor lfo; */
/* u8 lfoWaveshape; */

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

#define FM_VOICE_NOPS 4

void module_init(void) {
  // init module/param descriptor
  pDacsData = (fmsynthData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "fmsynth");

  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(cvSlew[0]), 0 );
  filter_1p_lo_init( &(cvSlew[1]), 0 );
  filter_1p_lo_init( &(cvSlew[2]), 0 );
  filter_1p_lo_init( &(cvSlew[3]), 0 );

  fm_voice_init(&voice, FM_VOICE_NOPS);

  param_setup( eParam_cvSlew0, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew1, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew2, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew3, PARAM_CV_SLEW_DEFAULT );

  param_setup( eParam_cvVal0, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal1, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal2, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal3, PARAM_CV_VAL_DEFAULT );

  param_setup( 	eParam_op1Vol, FADER_DEFAULT );
  param_setup( 	eParam_op1Pan, PAN_DEFAULT );

  param_setup( 	eParam_op2Vol, FADER_DEFAULT );
  param_setup( 	eParam_op2Pan, PAN_DEFAULT );

  param_setup( 	eParam_op3Vol, FADER_DEFAULT );
  param_setup( 	eParam_op3Pan, PAN_DEFAULT );

  param_setup( 	eParam_op4Vol, FADER_DEFAULT );
  param_setup( 	eParam_op4Pan, PAN_DEFAULT );

  param_setup( eParam_noteHz, 220 << 16 );
  param_setup( eParam_noteTune, FIX16_ONE );
  param_setup( eParam_noteTrigger, 0);
  param_setup( eParam_noteVelocity, PARAM_AMP_0 >> 2);

  param_setup(eParam_op1Mod1Source, 0);
  param_setup(eParam_op1Mod1Gain,0);
  param_setup(eParam_op1Mod2Source, 0);
  param_setup(eParam_op1Mod2Gain, 0);
  param_setup(eParam_op1Tune, FIX16_ONE);
  param_setup(eParam_op1Attack, SLEW_1S);
  param_setup(eParam_op1Decay, SLEW_1S);
  param_setup(eParam_op1Sustain, PARAM_AMP_0 >> 2);
  param_setup(eParam_op1Release, SLEW_1S);
  param_setup(eParam_op1Waveshape, 0);

  param_setup(eParam_op2Mod1Source, 0);
  param_setup(eParam_op2Mod1Gain,PARAM_AMP_0 >> 3);
  param_setup(eParam_op2Mod2Source, 0);
  param_setup(eParam_op2Mod2Gain, 0);
  param_setup(eParam_op2Tune, FIX16_ONE);
  param_setup(eParam_op2Attack, 0x00320000);
  param_setup(eParam_op2Decay, 0x00320000);
  param_setup(eParam_op2Sustain, PARAM_AMP_0);
  param_setup(eParam_op2Release, 0x00320000);
  param_setup(eParam_op2Waveshape, 0);

  param_setup(eParam_op3Mod1Source, 0);
  param_setup(eParam_op3Mod1Gain,PARAM_AMP_0 >> 2);
  param_setup(eParam_op3Mod2Source, 0);
  param_setup(eParam_op3Mod2Gain, PARAM_AMP_0 >> 2);
  param_setup(eParam_op3Tune, FIX16_ONE);
  param_setup(eParam_op3Attack, 0x00320000);
  param_setup(eParam_op3Decay, 0x00320000);
  param_setup(eParam_op3Sustain, PARAM_AMP_0 >> 2);
  param_setup(eParam_op3Release, 0x00320000);
  param_setup(eParam_op3Waveshape, 0);

  param_setup(eParam_op4Mod1Source, 0);
  param_setup(eParam_op4Mod1Gain,PARAM_AMP_0 >> 2);
  param_setup(eParam_op4Mod2Source, 0);
  param_setup(eParam_op4Mod2Gain, PARAM_AMP_0 >> 2);
  param_setup(eParam_op4Tune, FIX16_ONE);
  param_setup(eParam_op4Attack, 0x00320000);
  param_setup(eParam_op4Decay, 0x00320000);
  param_setup(eParam_op4Sustain, PARAM_AMP_0 >> 2);
  param_setup(eParam_op4Release, 0x00320000);
  param_setup(eParam_op4Waveshape, 0);

  param_setup (eParam_lfoSpeed, 0x00640000);
  param_setup (eParam_lfoWaveshape, PAN_DEFAULT);

}

// de-init
void module_deinit(void) {
}

void module_process_frame(void) {

  // Update one of the CV outputs
  if(filter_1p_sync(&(cvSlew[cvChan]))) { ;; } else {
    cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
    cv_update(cvChan, cvVal[cvChan]);
  }

  // Queue up the next CV output for processing next audio frame
  if(++cvChan == 4) {
    cvChan = 0;
  }
  fm_voice_next(&voice);
  int i;
  for(i=0; i<FM_VOICE_NOPS; i++) {
    mix_panned_mono (voice.opOutputs[i], &(out[0]), &(out[1]), opPans[i], opVols[i]);
  }
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE; // which one it this?
  switch(idx) {

  case eParam_cvVal0 :
    filter_1p_lo_in(&(cvSlew[0]), v);
    break;
  case eParam_cvVal1 :
    filter_1p_lo_in(&(cvSlew[1]), v);
    break;
  case eParam_cvVal2 :
    filter_1p_lo_in(&(cvSlew[2]), v);
    break;
  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), v);
    break;

  case eParam_cvSlew0 :
    filter_1p_lo_set_slew(&(cvSlew[0]), v);
    break;
  case eParam_cvSlew1 :
    filter_1p_lo_set_slew(&(cvSlew[1]), v);
    break;
  case eParam_cvSlew2 :
    filter_1p_lo_set_slew(&(cvSlew[2]), v);
    break;
  case eParam_cvSlew3 :
    filter_1p_lo_set_slew(&(cvSlew[3]), v);
    break;
  case eParam_op1Vol :
    opVols[0] = v;
    break;
  case eParam_op1Pan :
    opPans[0] = v;
    break;
  case eParam_op2Vol :
    opVols[2] = v;
    break;
  case eParam_op2Pan :
    opPans[1] = v;
    break;
  case eParam_op3Vol :
    opVols[2] = v;
    break;
  case eParam_op3Pan :
    opPans[2] = v;
    break;
  case eParam_op4Vol :
    opVols[3] = v;
    break;
  case eParam_op4Pan :
    opPans[3] = v;
    break;
  case eParam_noteHz :
    voice.noteHz = v;
    break;
  case eParam_noteTune :
    voice.noteTune = v;
    break;
  case eParam_noteTrigger :
    if (v > 0) {
      fm_voice_press(&voice);
    }
    else {
      fm_voice_release(&voice);
    }
    break;
  case eParam_noteVelocity :
    voice.noteLevel = v;
    break;

  case eParam_op1Mod1Source :
    voice.opMod1Source[0] = v >> 16;
    break;
  case eParam_op1Mod1Gain :
    voice.opMod1Gain[0] = v;
    break;
  case eParam_op1Mod2Source :
    voice.opMod2Source[0] = v >> 16;
    break;
  case eParam_op1Mod2Gain :
    voice.opMod2Gain[0] = v;
    break;
  case eParam_op1Tune :
    voice.opTune[0] = v;
    break;
  case eParam_op1Attack :
    voice.opEnv[0].attackTime = v;
    break;
  case eParam_op1Decay :
    voice.opEnv[0].decayTime = v;
    break;
  case eParam_op1Sustain :
   voice.opEnv[0].sustainLevel = v;
   break;
  case eParam_op1Release :
    voice.opEnv[0].releaseTime = v;
    break;
  case eParam_op1Waveshape :
    voice.opWaveshape[0] = v >> 16;
    break;

  case eParam_op2Mod1Source :
    voice.opMod1Source[1] = v >> 16;
    break;
  case eParam_op2Mod1Gain :
    voice.opMod1Gain[1] = v;
    break;
  case eParam_op2Mod2Source :
    voice.opMod2Source[1] = v >> 16;
    break;
  case eParam_op2Mod2Gain :
    voice.opMod2Gain[1] = v;
    break;
  case eParam_op2Tune :
    voice.opTune[1] = v;
    break;
  case eParam_op2Attack :
    voice.opEnv[1].attackTime = v;
    break;
  case eParam_op2Decay :
    voice.opEnv[1].decayTime = v;
    break;
  case eParam_op2Sustain :
   voice.opEnv[1].sustainLevel = v;
   break;
  case eParam_op2Release :
    voice.opEnv[1].releaseTime = v;
    break;
  case eParam_op2Waveshape :
    voice.opWaveshape[1] = v >> 16;
    break;

  case eParam_op3Mod1Source :
    voice.opMod1Source[2] = v >> 16;
    break;
  case eParam_op3Mod1Gain :
    voice.opMod1Gain[2] = v;
    break;
  case eParam_op3Mod2Source :
    voice.opMod2Source[2] = v >> 16;
    break;
  case eParam_op3Mod2Gain :
    voice.opMod2Gain[2] = v;
    break;
  case eParam_op3Tune :
    voice.opTune[2] = v;
    break;
  case eParam_op3Attack :
    voice.opEnv[2].attackTime = v;
    break;
  case eParam_op3Decay :
    voice.opEnv[2].decayTime = v;
    break;
  case eParam_op3Sustain :
   voice.opEnv[2].sustainLevel = v;
   break;
  case eParam_op3Release :
    voice.opEnv[2].releaseTime = v;
    break;
  case eParam_op3Waveshape :
    voice.opWaveshape[2] = v >> 16;
    break;

  case eParam_op4Mod1Source :
    voice.opMod1Source[3] = v >> 16;
    break;
  case eParam_op4Mod1Gain :
    voice.opMod1Gain[3] = v;
    break;
  case eParam_op4Mod2Source :
    voice.opMod2Source[3] = v >> 16;
    break;
  case eParam_op4Mod2Gain :
    voice.opMod2Gain[3] = v;
    break;
  case eParam_op4Tune :
    voice.opTune[3] = v;
    break;
  case eParam_op4Attack :
    voice.opEnv[3].attackTime = v;
    break;
  case eParam_op4Decay :
    voice.opEnv[3].decayTime = v;
    break;
  case eParam_op4Sustain :
   voice.opEnv[3].sustainLevel = v;
   break;
  case eParam_op4Release :
    voice.opEnv[3].releaseTime = v;
    break;
  case eParam_op4Waveshape :
    voice.opWaveshape[3] = v >> 16;
    break;

  case eParam_lfoSpeed :
    voice.lfo.freq = shl_fr1x32(v, -4);
    break;
  case eParam_lfoWaveshape :
    voice.lfoWaveshape = v;
    break;

  default:
    break;
  }
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}
