/* analyser.c
   aleph-bfin

   simplest CV output module.
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
#include "ricks_tricks.h"
#include "cv.h"
#include "gpio.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio
#include "filter_1p.h"
#include "module.h"

/// custom
#include "params.h"

// data structure of external memory
typedef struct _analyserData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} analyserData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
analyserData* pAnalyserData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

trackingEnvelopeLin line;
trackingEnvelopeLog loge;
pitchDetector pd;

void module_init(void) {
  // init module/param descriptor
  pAnalyserData = (analyserData*)SDRAM_ADDRESS;

  gModuleData = &(pAnalyserData->super);
  strcpy(gModuleData->name, "analyser");

  gModuleData->paramData = (ParamData*)pAnalyserData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(cvSlew[0]), 0 );

  param_setup( eParam_cvSlew3, PARAM_CV_SLEW_DEFAULT );

  param_setup( eParam_cvVal3, PARAM_CV_VAL_DEFAULT );

  param_setup( eParam_linAttack, LINSLEW_10MS);
  param_setup( eParam_linDecay, LINSLEW_100MS);

  param_setup( eParam_logAttack, SLEW_10MS);
  param_setup( eParam_logDecay, SLEW_100MS);
  pitchDetector_init(&pd);
  trackingEnvelopeLin_init(&line);
  trackingEnvelopeLog_init(&loge);
}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}

//// FIXME:
/* for now, stagger DAC channels across consecutive audio frames
   better method might be:
   - enable DMA tx interrupt
   - each ISR calls the next channel to be loaded
   - last thing audio ISR does is call the first DAC channel to be loaded
   - dac_update writes to 4x16 volatile buffer
*/
void module_process_frame(void) {
  static long int ledCount = 0;

  if(ledCount > 6000) {
    LED3_TOGGLE;
    LED4_TOGGLE;
    ledCount = 0;
  }
  ledCount++;
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];
  out[3] = in[3];

  fract32 sig_bus = add_fr1x32(in[0], in[1]);
  cvVal[0] = trackingEnvelopeLin_next(&line, sig_bus);
  param_setup( eParam_linEnv, cvVal[0] );

  cvVal[1] = trackingEnvelopeLog_next(&loge, sig_bus);
  param_setup(eParam_logEnv, cvVal[1]);
  cvVal[2] = shl_fr1x32(pitchTrack(&pd, sig_bus), 14);;
  param_setup(eParam_pitch, cvVal[2]);
  cvVal[3] = filter_1p_lo_next(&(cvSlew[3]));

  // Update one of the CV outputs
  cv_update(cvChan, cvVal[cvChan]);

  // Queue up the next CV output for processing next audio frame
  if(++cvChan == 4) {
    cvChan = 0;
  }
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE; // which one it this?
  switch(idx) {

  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), v);
    break;

  case eParam_cvSlew3 :
    filter_1p_lo_set_slew(&(cvSlew[3]), v);
    break;
  case eParam_linAttack :
    line.slew.up = v;
    break;
  case eParam_linDecay :
    line.slew.down = v;
    break;
  case eParam_logAttack :
    loge.up = v;
    break;
  case eParam_logDecay :
    loge.down = v;
    break;
  default:
    break;
  }
}
