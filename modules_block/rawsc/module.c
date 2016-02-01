#include <string.h>

#include "module.h"
#include "osc.h"
#include "params.h"

ModuleData* gModuleData;
static ModuleData super;
static ParamData mParamData[eParamNumParams];

static inline void param_setup(u32 id, ParamValue v) {
  // set the input data so that bfin core will report it
  // back to the controller via SPI, when requested. 
  // (bees will make such a request of each param at launch to sync with network.)
  // this is also how a polled analysis parameter would work.
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


void module_init(void) {
  u16 i;

  // detune!
  param_setup( eParamFreqFine0, 0x00000000);
  param_setup( eParamFreqFine1, 0x000f0000);
  param_setup( eParamFreqFine2, 0x00ff0000);
  param_setup( eParamFreqFine3, 0x7fff0000);

  // ~220hz
  param_setup( eParamFreqCoarse0, 0x00962fc9); 
  param_setup( eParamFreqCoarse1, 0x00962fc9); 
  param_setup( eParamFreqCoarse2, 0x00962fc9); 
  param_setup( eParamFreqCoarse3, 0x00962fc9); 
  
  param_setup( eParamAmp0, 0x3fffffff / NUM_OSCS);
  param_setup( eParamAmp1, 0x3fffffff / NUM_OSCS);
  param_setup( eParamAmp2, 0x3fffffff / NUM_OSCS);
  param_setup( eParamAmp3, 0x3fffffff / NUM_OSCS);

  for(i=0; i<NUM_OSCS; i++) { 
    osc_set_phase(i, 0);
  }

  gModuleData = &super;
  strcpy(gModuleData->name, "rawsc");
  gModuleData->paramData = mParamData;
  gModuleData->numParams = eParamNumParams;
}

void module_process_block(buffer_t *inChannels, buffer_t *outChannels) { 
  u16 i, j;
  
  for(i=0; i<AUDIO_CHANNELS; i++) {
    for(j=0; j<AUDIO_FRAMES; j++) {
      (*outChannels)[i][j] = (*inChannels)[i][j];
    }
  }

  for(i=0; i<NUM_OSCS; i++) { 
    osc_process_block(i, outChannels);
  }
}

void module_set_param(u32 idx, ParamValue val) {
    switch(idx) {

    case eParamAmp0:
      osc_set_amp(0, val );
      break;

    case eParamFreqCoarse0:
      osc_set_phi_upper(0, val );
      break;

    case eParamFreqFine0:
      osc_set_phi_lower(0, val );
      break;

      
    case eParamAmp1:
      osc_set_amp(1, val );
      break;

    case eParamFreqCoarse1:
      osc_set_phi_upper(1, val );
      break;

    case eParamFreqFine1:
      osc_set_phi_lower(1, val );
      break;

      
    case eParamAmp2:
      osc_set_amp(2, val );
      break;

    case eParamFreqCoarse2:
      osc_set_phi_upper(2, val );
      break;

    case eParamFreqFine2:
      osc_set_phi_lower(2, val );
      break;

      
    case eParamAmp3:
      osc_set_amp(3, val );
      break;

    case eParamFreqCoarse3:
      osc_set_phi_upper(3, val );
      break;

    case eParamFreqFine3:
      osc_set_phi_lower(3, val );
      break;


    }

}
