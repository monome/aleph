#include "module.h"
#include "osc.h"
#include "params.h"

ModuleData* gModuleData;


void module_init(void) {
  osc_set_phi(0x00962fc9); // ~220 hz
  osc_set_amp(0x3fffffff);
}

void module_process_block(buffer_t *inChannels, buffer_t *outChannels,
			  const u8 numChannels, const u8 numFrames) {
  u16 i, j;
  
  for(i=0; i<numChannels; i++) {
    for(j=0; j<numFrames; j++) {
      (*outChannels)[i][j] = (*inChannels)[i][j];
    }
  }
  
  osc_process_block(outChannels, numChannels, numFrames);
}

void module_set_param(u32 idx, ParamValue val) {
  /// test
  //  return;
  
    switch(idx) {

    case eParamAmp0:
      osc_set_amp( val );
      break;

    case eParamFreq0:
      osc_set_phi( val << 1);
      break;

    }

}
