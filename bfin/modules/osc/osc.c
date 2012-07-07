#include <fract_math.h>
#include <string.h>
#include "bfin_core.h"
#include "param.h"
#include "module.h"
#include "osc_data.h"

// external data
oscData_t * data = (oscData_t *)SDRAM_ADDRESS;

// define twice for preprocesor and core code modules
const u32 kNumParams = MODULE_NUM_PARAMS;

//======== global function def
//----- init
void init_module(void) {

  strcpy(data->paramDesc[eParamAmp].name, "amp");
  strcpy(data->paramDesc[eParamAmp].unit, "amp");
  data->paramDesc[eParamAmp].type = eParamFract;

  strcpy(data->paramDesc[eParamFreq].name, "freq");
  strcpy(data->paramDesc[eParamFreq].unit, "hz");
  data->paramDesc[eParamFreq].type = eParamFloat;

}

//------ process frame
void process_frame(void) {
  out0 = in0;
  out1 = in1;
  out2 = in2;
  out3 = in3;
}
