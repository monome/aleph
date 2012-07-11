#include <fract_math.h>
#include <fract2float_conv.h>
#include <math.h>
#include <string.h>


#include "bfin_core.h"
#include "param.h"
#include "module.h"
#include "osc.h"

// external data, manually located at start of SDRAM
oscData_t * data = (oscData_t *)SDRAM_ADDRESS;
// pointer to data superclass (for access by bfin core routines)
moduleData_t* moduleData;

//---------------------------
//---- static vars
// current phase
static f32 phase = 0.f;
// constant representing phase change for 1hz
static const f32 rho = (f32)WAVETABLE_NUM / (f32)SAMPLERATE;
// phase increment
static f32 phaseInc;
// output value
static fract32 output = 0;

//---------------------------
//----- static functions
static void init_wavetable(void) {
  const f32 inc = M_TWOPI / (f32)WAVETABLE_NUM;
  f32 x = 0.f;
  u16 i;
  for (i=0; i<WAVETABLE_NUM; i++) {
    data->sineTable[i] = sinf(x);
    x += inc;
  }
}

// linear-interpolated lookup
static fract32 wavetable_interp(f32 phase) {
  const s16 x = (s16)phase;
  const f32 fx = phase - x;
  s16 xnext = x + 1;
  f32 y;
  if ( xnext > WAVETABLE_NUM_1) {
    xnext -= WAVETABLE_NUM_1;
  }
  y = data->sineTable[x] * (1.f - fx);
  y += data->sineTable[xnext] * fx;
  return float_to_fr32(y);
}

//----- init
void init_module(void) {
  // set the superclass pointer
  moduleData = &(data->super);

  // initialize superclass internals
  moduleData->paramDesc = data->paramDesc_p;  
  moduleData->paramData = data->paramData_p;  
  moduleData->numParams = MODULE_NUM_PARAMS;

  // initialize parameter descriptors
  strcpy(moduleData->paramDesc[eParamAmp].label, "amp");
  strcpy(moduleData->paramDesc[eParamAmp].unit, "amp");
  moduleData->paramDesc[eParamAmp].type = PARAM_TYPE_FRACT;
  moduleData->paramDesc[eParamAmp].min = 0.f;
  moduleData->paramDesc[eParamAmp].max = 1.f;

  strcpy(moduleData->paramDesc[eParamFreq].label, "freq");
  strcpy(moduleData->paramDesc[eParamFreq].unit, "hz");
  moduleData->paramDesc[eParamFreq].type = PARAM_TYPE_FLOAT;
  moduleData->paramDesc[eParamFreq].min = 20.f;
  moduleData->paramDesc[eParamFreq].max = 10000.f;

  // initialize parameter values
  moduleData->paramData[eParamAmp].value.asFract = FRACT32_MAX >> 3;
  moduleData->paramData[eParamFreq].value.asFloat = 440;

  // initialize wavetable
  init_wavetable();

  // intial phase increment
  phaseInc = rho * 440.f;
}

//------ process frame
void process_frame(void) {
  // check  if freq param changed
  if(moduleData->paramData[eParamFreq].changed) {
    moduleData->paramData[eParamFreq].changed = 0;
    phaseInc = moduleData->paramData[eParamFreq].value.asFloat * rho;
  }
  phase += phaseInc;
  while (phase > (f32)WAVETABLE_NUM_1) {
    phase -= (f32)WAVETABLE_NUM_1;
  }
  output = mult_fr1x32x32(moduleData->paramData[eParamAmp].value.asFract,
			  wavetable_interp(phase));
  out0 = output;
  out1 = output;
  out2 = output;
  out3 = output;
}
