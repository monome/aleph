/* dacs.c
   aleph-bfin

   pitchshift module with 4 'grains'

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
//#include "dac.h"
#include "gpio.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio
#include "filter_1p.h"
#include "module.h"
#include "pitch_shift.h"

/// custom
#include "params.h"


// total SDRAM is 64M
// each line 16 bit address
#define LINES_BUF_FRAMES 0x10000
//#define LINES_BUF_FRAMES 0xFFFFFF
// try...
//#define LINES_BUF_FRAMES 0x600000
//#define LINES_BUF_FRAMES 0x1000000
//#define LINES_BUF_FRAMES 0xbb8000 // 256 seconds @ 48k
#define NGRAINS 4
#define PARAM_SECONDS_MAX 0x003c0000

pitchShift grains[NGRAINS];

ParamValue auxL[4];
ParamValue auxR[4];
ParamValue auxLTarget[4];
ParamValue auxRTarget[4];
#define AUX_DEFAULT PARAM_AMP_0

ParamValue pan[4];
ParamValue panTarget[4];
#define PAN_DEFAULT PAN_MAX/2

ParamValue fader[4];
ParamValue faderTarget[4];
#define FADER_DEFAULT PARAM_AMP_0

ParamValue effect[4];
ParamValue effectTarget[4];
#define EFFECT_DEFAULT PARAM_AMP_0
//ParamValue eq_hi[4];
//ParamValue eq_mid[4];
//ParamValue eq_lo[4];

ParamValue pitchShiftFader[NGRAINS];
ParamValue pitchShiftFaderTarget[NGRAINS];

ParamValue pitchFactor[NGRAINS] = {0,0,0,0};
ParamValue pitchFactorTarget[NGRAINS] = {0,0,0,0};
/* static filter_1p_lo pitchFactorSlew[NGRAINS] ; */

ParamValue feedback=0;
ParamValue feedbackTarget=0;

// data structure of external memory
typedef struct _grainsData {
  ModuleData super;
  //ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
  volatile fract32 audioBuffer[NGRAINS][LINES_BUF_FRAMES];
} grainsData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;
//-----------------------
//------ static variables

// pointer to all external memory
grainsData* pGrainsData;

// dac values (u16, but use fract32 and audio integrators)

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}
void module_init(void) {

  // init module/param descriptor
  pGrainsData = (grainsData*)SDRAM_ADDRESS;

  gModuleData = &(pGrainsData->super);
  strcpy(gModuleData->name, "grains");

  gModuleData->paramData = (ParamData*)pGrainsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  param_setup( 	eParam_auxL0,		AUX_DEFAULT );
  param_setup( 	eParam_auxR0,		AUX_DEFAULT );
  param_setup( 	eParam_pan0,		PAN_DEFAULT );
  param_setup( 	eParam_fader0,		FADER_DEFAULT );
  param_setup( 	eParam_effect0,		EFFECT_DEFAULT );

  param_setup( 	eParam_auxL1,		AUX_DEFAULT );
  param_setup( 	eParam_auxR1,		AUX_DEFAULT );
  param_setup( 	eParam_pan1,		PAN_DEFAULT );
  param_setup( 	eParam_fader1,		FADER_DEFAULT );
  param_setup( 	eParam_effect1,		EFFECT_DEFAULT );

  param_setup( 	eParam_auxL2,		AUX_DEFAULT );
  param_setup( 	eParam_auxR2,		AUX_DEFAULT );
  param_setup( 	eParam_pan2,		PAN_DEFAULT );
  param_setup( 	eParam_fader2,		FADER_DEFAULT );
  param_setup( 	eParam_effect2,		EFFECT_DEFAULT );

  param_setup( 	eParam_auxL3,		AUX_DEFAULT );
  param_setup( 	eParam_auxR3,		AUX_DEFAULT );
  param_setup( 	eParam_pan3,		PAN_DEFAULT );
  param_setup( 	eParam_fader3,		FADER_DEFAULT );
  param_setup( 	eParam_effect3,		EFFECT_DEFAULT );

  pitchShift_init(&(grains[0]), pGrainsData->audioBuffer[0], LINES_BUF_FRAMES);
  pitchShift_init(&(grains[1]), pGrainsData->audioBuffer[1], LINES_BUF_FRAMES);
  pitchShift_init(&(grains[2]), pGrainsData->audioBuffer[2], LINES_BUF_FRAMES);
  pitchShift_init(&(grains[3]), pGrainsData->audioBuffer[3], LINES_BUF_FRAMES);

  /* filter_1p_lo_init( &(pitchFactorSlew[0]), 0 ); */
  /* filter_1p_lo_init( &(pitchFactorSlew[1]), 0 ); */
  /* filter_1p_lo_init( &(pitchFactorSlew[2]), 0 ); */
  /* filter_1p_lo_init( &(pitchFactorSlew[3]), 0 ); */
  /* filter_1p_lo_init( &(pitchFactorSlew[4]), 0 ); */

  //param_setup( 	eParam_pitchshift0,		0 );
  param_setup( 	eParam_feedback, FADER_DEFAULT );
  param_setup( 	eParam_feedback, 0 );

  param_setup( 	eParam_pitchshift0fader, FADER_DEFAULT );
  param_setup( 	eParam_pitchshift1fader, FADER_DEFAULT );
  param_setup( 	eParam_pitchshift2fader, FADER_DEFAULT );
  param_setup( 	eParam_pitchshift3fader, FADER_DEFAULT );
}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}

void mix_aux_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) ;

void mix_panned_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) ;

#define simple_slew(x, y) x = y/100 + x/100 * 99

#define simple_busmix(x, y, fact) x = add_fr1x32(x, mult_fr1x32x32(y, fact))

fract32 delayOutput, delayInput;
void module_process_frame(void) {

  u8 i;
  //IIR slew
  for (i=0;i<4;i++) {
    simple_slew(auxL[i], auxLTarget[i]);
    simple_slew(auxR[i], auxRTarget[i]);
    simple_slew(pan[i], panTarget[i]);
    simple_slew(fader[i], faderTarget[i]);
    simple_slew(effect[i],effectTarget[i]);
    simple_slew(pitchShiftFader[i], pitchShiftFaderTarget[i]);
  }
  simple_slew(feedbackTarget, feedback);
  
  //define delay input & output

  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  delayInput = 0;

  for (i=0;i<4;i++) {
    mix_panned_mono(in[i], &(out[0]), &(out[1]), pan[i], fader[i]);
    mix_aux_mono(in[i], &(out[2]), &(out[3]), auxL[i], auxR[i]);
    simple_busmix(delayInput, in[i],effect[i]);
  }
  simple_busmix (delayInput, delayOutput,feedback);
  delayOutput = 0;
  for (i=0;i<4;i++) {
    simple_busmix (delayOutput, pitchShift_next(&(grains[i]), delayInput), pitchShiftFader[i]);
  }
  mix_panned_mono(delayOutput, &(out[0]), &(out[1]),PAN_DEFAULT ,FADER_DEFAULT );
}

void mix_aux_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue left_value, ParamValue right_value) {
    *out_right = add_fr1x32(*out_right,mult_fr1x32x32(in_mono, right_value));
    *out_left = add_fr1x32(*out_left,mult_fr1x32x32(in_mono, left_value));
}


void mix_panned_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) {
    fract32 pan_factor, post_fader;

    pan_factor = (fract32) ( pan );
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_left = add_fr1x32(*out_left, mult_fr1x32x32(in_mono, post_fader));

    pan_factor = (fract32) ( PAN_MAX - pan );
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_right = add_fr1x32(*out_right, mult_fr1x32x32(in_mono, post_fader));

}


// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE;
  switch(idx) {
    // dac values
  case eParam_auxL0 :
    auxLTarget[0] = v;
    break;
  case eParam_auxR0 :
    auxRTarget[0] = v;
    break;
  case eParam_pan0 :
    panTarget[0] = v;
    break;
  case eParam_fader0 :
    faderTarget[0] = v;
    break;
  case eParam_effect0 :
    effectTarget[0] = v;
    break;
  case eParam_auxL1 :
    auxLTarget[1] = v;
    break;
  case eParam_auxR1 :
    auxRTarget[1] = v;
    break;
  case eParam_pan1 :
    panTarget[1] = v;
    break;
  case eParam_fader1 :
    faderTarget[1] = v;
    break;
  case eParam_effect1 :
    effectTarget[1] = v;
    break;
  case eParam_auxL2 :
    auxLTarget[2] = v;
    break;
  case eParam_auxR2 :
    auxRTarget[2] = v;
    break;
  case eParam_pan2 :
    panTarget[2] = v;
    break;
  case eParam_fader2 :
    faderTarget[2] = v;
    break;
  case eParam_effect2 :
    effectTarget[2] = v;
    break;
  case eParam_auxL3 :
    auxLTarget[3] = v;
    break;
  case eParam_auxR3 :
    auxRTarget[3] = v;
    break;
  case eParam_pan3 :
    panTarget[3] = v;
    break;
  case eParam_fader3 :
    faderTarget[3] = v;
    break;
  case eParam_effect3 :
    effectTarget[3] = v;
    break;
  case eParam_feedback :
    feedbackTarget = v;
    break;

  case eParam_pitchshift0 :
    pitchShift_set_pitchFactor24_8(&(grains[0]), v/256);
    break;
  case eParam_pitchshift0fader:
    pitchShiftFaderTarget[0] = v;
    break;

  case eParam_pitchshift1 :
    pitchShift_set_pitchFactor24_8(&(grains[1]), v/256);
    break;
  case eParam_pitchshift1fader:
    pitchShiftFaderTarget[1] = v;
    break;

  case eParam_pitchshift2 :
    pitchShift_set_pitchFactor24_8(&(grains[2]), v/256);
    break;
  case eParam_pitchshift2fader:
    pitchShiftFaderTarget[2] = v;
    break;

  case eParam_pitchshift3 :
    pitchShift_set_pitchFactor24_8(&(grains[3]), v/256);
    break;
  case eParam_pitchshift3fader:
    pitchShiftFaderTarget[3] = v;
    break;

  default:
    break;
  }
}
