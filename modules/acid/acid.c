/* svfnoise.c
   aleph-bfin
   
   noise generator + input + state variable filter

*/

// std
#include <stdlib.h>
#include <string.h>
// aleph-common
#include "fix.h"
// aleph-audio
#include "conversion.h"
#include "env.h"
#include "filter_svf.h"
//#include "noise.h"
#include "noise_acid.h"

// bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "params.h"
#include "types.h"

#include "dsyn_voice.h"
#include "acid.h"


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _drumsData {
  ModuleData super;
  ParamData mParamData[ACID_NPARAMS];
} drumsynData;

//-------------------------
//----- extern vars (initialized here)
ModuleData * gModuleData; // module data

//-----------------------
//------ static variables
drumsynVoice* voices[DRUMSYN_NVOICES];
monosynthVoice mVoices[MONOSYNTH_NVOICES];

// pointer to local module data, initialize at top of SDRAM
static drumsynData * data;
//-- static allocation (SRAM) for variables that are small and/or frequently accessed:

fract32 dsynLevels[DRUMSYN_NVOICES];
fract32 monosynthLevels[MONOSYNTH_NVOICES];

//----------------------
//----- external functions

void module_init(void) {
  u8 i;
  // init module/param descriptor
  // intialize local data at start of SDRAM
  data = (drumsynData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
  gModuleData = &(data->super);
  strcpy(gModuleData->name, "acid");

  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = ACID_NPARAMS;

  for(i=0; i<DRUMSYN_NVOICES; i++) {
    voices[i] = (drumsynVoice*)malloc(sizeof(drumsynVoice));
    drumsyn_voice_init(voices[i]);
    dsynLevels[i] = 0;
  }

  for(i=0; i < MONOSYNTH_NVOICES; i++) {
    monosynthVoice_init(&mVoices[i]);
    monosynthLevels[i] = 0;
  }

  // setup params with default values
  params_default();

}

void module_deinit(void) {
}



// get number of parameters
extern u32 module_get_num_params(void) {
  return ACID_NPARAMS;
}

// frame callback
void module_process_frame(void) {
  // sum input pairs to output pairs
  fract32 sum01 = add_fr1x32(in[0], in[1]);
  fract32 sum23 = add_fr1x32(in[2], in[3]);
  fract32 synthOutput = 0;
  fract32 synthBus = 0;

  int i;
  for(i=0; i < DRUMSYN_NVOICES; i++) {
    synthOutput = drumsyn_voice_next(voices[i]);
    synthOutput = mult_fr1x32x32(synthOutput, dsynLevels[i]);
    synthOutput = shl_fr1x32(synthOutput, 2);
    synthBus = add_fr1x32(synthBus, synthOutput);
  }
  for(i=0; i < MONOSYNTH_NVOICES; i++) {
    synthOutput = monosynthVoice_next(&mVoices[i]);
    synthOutput = mult_fr1x32x32(synthOutput, monosynthLevels[i]);
    synthOutput = shl_fr1x32(synthOutput, 1);
    synthBus = add_fr1x32(synthBus, synthOutput);
  }

  sum01 = add_fr1x32(sum01, synthBus);
  sum23 = add_fr1x32(sum23, synthBus);

  out[0] = sum01;
  out[1] = sum01;
  out[2] = sum23;
  out[3] = sum23;
}
