#include "module.h"
#include "osc.h"
#include "params.h"

ModuleData* gModuleData;


void module_init(void) {
  u16 i;
  for(i=0; i<NUM_OSCS; i++) { 
    osc_set_phase(i, 0);
    osc_set_phi(i, 0x00962fc9); // ~220 hz
    osc_set_amp(0, 0x3fffffff / NUM_OSCS);
  }
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
