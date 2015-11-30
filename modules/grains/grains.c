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
#include "grain.h"

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
#define PARAM_SECONDS_MAX 0x003c0000
#define AUX_DEFAULT PARAM_AMP_0
#define PAN_DEFAULT PAN_MAX/2
#define FADER_DEFAULT PARAM_AMP_0
#define EFFECT_DEFAULT PARAM_AMP_0

//ADC mix params
ParamValue faderI[4];
ParamValue faderITarget[4];

ParamValue panI[4];
ParamValue panITarget[4];

ParamValue aux1I[4];
ParamValue aux1ITarget[4];

ParamValue aux2I[4];
ParamValue aux2ITarget[4];

ParamValue effectI[4];
ParamValue effectITarget[4];

#define NGRAINS 1
grain grains[NGRAINS];
//Grain mix params

ParamValue faderG[NGRAINS];
ParamValue faderGTarget[NGRAINS];

ParamValue panG[NGRAINS];
ParamValue panGTarget[NGRAINS];

ParamValue aux1G[NGRAINS];
ParamValue aux1GTarget[NGRAINS];

ParamValue aux2G[NGRAINS];
ParamValue aux2GTarget[NGRAINS];

ParamValue effectG[NGRAINS];
ParamValue effectGTarget[NGRAINS];

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

  param_setup( 	eParam_fader_i1,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i1,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i1,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i1,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i1,	EFFECT_DEFAULT );

  param_setup( 	eParam_fader_i2,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i2,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i2,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i2,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i2,	EFFECT_DEFAULT );

  param_setup( 	eParam_fader_i3,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i3,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i3,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i3,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i3,	EFFECT_DEFAULT );

  param_setup( 	eParam_fader_i4,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i4,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i4,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i4,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i4,	EFFECT_DEFAULT );

  param_setup (eParam_scrubFadeLength,  45);
  param_setup (eParam_echoFadeLength,   45);

  param_setup( 	eParam_fader_g1,	FADER_DEFAULT );
  param_setup( 	eParam_pan_g1,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_g1,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_g1,		AUX_DEFAULT );
  param_setup( 	eParam_effect_g1,	EFFECT_DEFAULT );

  param_setup (eParam_scrubPitch_g1,    256);
  grain_init(&(grains[0]), pGrainsData->audioBuffer[0], LINES_BUF_FRAMES);
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
//#define simple_slew(x, y) x = y


#define simple_busmix(x, y, fact) x = add_fr1x32(x, mult_fr1x32x32(y, fact))

fract32 effectBus;
fract32 grainOut;

void module_process_frame(void) {

  u8 i;
  //IIR slew
  for (i=0;i<4;i++) {
    simple_slew(faderI[i], faderITarget[i]);
    simple_slew(aux1I[i], aux1ITarget[i]);
    simple_slew(aux2I[i], aux2ITarget[i]);
    simple_slew(panI[i], panITarget[i]);
    simple_slew(effectI[i],effectITarget[i]);
  }
  for (i=0;i<NGRAINS;i++) {
    simple_slew(faderG[i], faderGTarget[i]);
  }
  
  //define delay input & output

  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  return ;
  for (i=0;i<4;i++) {
    mix_panned_mono (in[i], &(out[0]), &(out[1]), panI[i], faderI[i]);
    mix_aux_mono (in[i], &(out[2]), &(out[3]), aux1I[i], aux2I[i]);
    simple_busmix (effectBus, in[i],effectI[i]);
  }

  effectBus = 0;
  for (i=0;i<NGRAINS;i++) {
    grainOut=grain_next(&(grains[i]), effectBus);
    mix_panned_mono (grainOut, &(out[0]), &(out[1]), panG[i], faderG[i]);
    mix_aux_mono (grainOut, &(out[2]), &(out[3]), aux1G[i], aux2G[i]);
    simple_busmix (grainOut, in[i],effectI[i]);
  }
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
    // ADC mix params
  case eParam_fader_i1 :
    faderITarget[0] = v;
    break;
  case eParam_pan_i1 :
    panITarget[0] = v;
    break;
  case eParam_aux1_i1 :
    aux1ITarget[0] = v;
    break;
  case eParam_aux2_i1 :
    aux2ITarget[0] = v;
    break;
  case eParam_effect_i1 :
    effectITarget[0] = v;
    break;

  case eParam_fader_i2 :
    faderITarget[1] = v;
    break;
  case eParam_pan_i2 :
    panITarget[1] = v;
    break;
  case eParam_aux1_i2 :
    aux1ITarget[1] = v;
    break;
  case eParam_aux2_i2 :
    aux2ITarget[1] = v;
    break;
  case eParam_effect_i2 :
    effectITarget[1] = v;
    break;

  case eParam_fader_i3 :
    faderITarget[2] = v;
    break;
  case eParam_pan_i3 :
    panITarget[2] = v;
    break;
  case eParam_aux1_i3 :
    aux1ITarget[2] = v;
    break;
  case eParam_aux2_i3 :
    aux2ITarget[2] = v;
    break;
  case eParam_effect_i3 :
    effectITarget[2] = v;
    break;

  case eParam_fader_i4 :
    faderITarget[3] = v;
    break;
  case eParam_pan_i4 :
    panITarget[3] = v;
    break;
  case eParam_aux1_i4 :
    aux1ITarget[3] = v;
    break;
  case eParam_aux2_i4 :
    aux2ITarget[3] = v;
    break;
  case eParam_effect_i4 :
    effectITarget[3] = v;
    break;

    //global grain params
  case eParam_scrubFadeLength :
    break;
  case eParam_echoFadeLength :
    break;

  //grain mix params
  case eParam_fader_g1 :
    faderGTarget[0] = v;
    break;
  case eParam_pan_g1 :
    panGTarget[0] = v;
    break;
  case eParam_aux1_g1 :
    aux1GTarget[0] = v;
    break;
  case eParam_aux2_g1 :
    aux2GTarget[0] = v;
    break;
  case eParam_effect_g1 :
    effectGTarget[0] = v;
    break;
    
  //grain scrubber params
  case eParam_scrubPitch_g1 :
    grain_set_pitchFactor24_8(&(grains[0]), v/256);
    break;

  case eParam_scrubLength_g1 :
    break;
  case eParam_scrubRandomise_g1 :
    break;
  case eParam_scrubEdgeBehaviour_g1 :
    break;

  //grain echo params
  case eParam_echoTime_g1 :
    break;
  case eParam_echoSpeed_g1 :
    break;
  case eParam_echoEdgeBehaviour_g1 :
    break;
  case eParam_echoMin_g1 :
    break;
  case eParam_echoMax_g1 :
    break;
  case eParam_echoLFOAmp_g1 :
    break;
  case eParam_echoLFOSpeed_g1 :
    break;
  default:
    break;
  }
}
