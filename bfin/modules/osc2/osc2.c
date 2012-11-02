// std
#include <math.h>
#include <string.h>
// bfin
#include <fract_math.h>
#include <fract2float_conv.h>

// aleph
#include "bfin_core.h"

// module
#include "osc2.h"

// external data, manually located at start of SDRAM
oscData_t * data = (oscData_t *)SDRAM_ADDRESS;
// pointer to data superclass (for access by bfin core routines)
moduleData_t* moduleData;

//---------------------------
//---- static vars
// current phase
static f32 phase1 = 0.f;
static f32 phase2 = 0.f;
// constant representing phase change for 1hz
static const f32 rho = (f32)WAVETABLE_NUM / (f32)SAMPLERATE;
// phase increment
static f32 phaseInc1;
static f32 phaseInc2;
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
static f32 wavetable_interp(f32 phase) {
  const s32 x = (s32)phase;
  const f32 fx = phase - x;
  s32 xnext = x + 1;
  f32 y;
  if ( xnext > WAVETABLE_NUM_1) {
    xnext -= WAVETABLE_NUM_1;
  }
  y = data->sineTable[x] * (1.f - fx);
  y += data->sineTable[xnext] * fx;
  return y;
}

//----- init
void init_module(void) {
  // set the superclass pointer
  moduleData = &(data->super);

  // initialize superclass internals
  moduleData->paramDesc = data->paramDescPr;  
  moduleData->paramData = data->paramDataPr;  
  moduleData->numParams = MODULE_NUM_PARAMS;

  /*
  // initialize parameter descriptors
  strcpy(moduleData->paramDesc[eParamAmp].label, "amp");
  strcpy(moduleData->paramDesc[eParamAmp].unit, "amp");
  moduleData->paramDesc[eParamAmp].type = PARAM_TYPE_FRACT;
  moduleData->paramDesc[eParamAmp].min = 0.f;
  moduleData->paramDesc[eParamAmp].max = 1.f;
  */

  strcpy(moduleData->paramDesc[eParamFreq1].label, "freq1");
  strcpy(moduleData->paramDesc[eParamFreq1].unit, "hz");
  moduleData->paramDesc[eParamFreq1].type = PARAM_TYPE_FLOAT;
  moduleData->paramDesc[eParamFreq1].min = 20.f;
  moduleData->paramDesc[eParamFreq1].max = 10000.f;

  strcpy(moduleData->paramDesc[eParamFreq2].label, "freq2");
  strcpy(moduleData->paramDesc[eParamFreq2].unit, "hz");
  moduleData->paramDesc[eParamFreq2].type = PARAM_TYPE_FLOAT;
  moduleData->paramDesc[eParamFreq2].min = 20.f;
  moduleData->paramDesc[eParamFreq2].max = 10000.f;

  // initialize parameter values
  // moduleData->paramData[eParamAmp].value.asFract = FRACT32_MAX >> 4;
  moduleData->paramData[eParamFreq1].value.asFloat = 220;
  moduleData->paramData[eParamFreq2].value.asFloat = 330;

  // initialize wavetable
  init_wavetable();

  // intial phase increment
  phaseInc1 = rho * moduleData->paramData[eParamFreq1].value.asFloat;
  phaseInc2 = rho * moduleData->paramData[eParamFreq2].value.asFloat;
}

//------ process frame
void process_frame(void) {

  /*
  // check  if freq params changed
  if(moduleData->paramData[eParamFreq1].changed) {
    moduleData->paramData[eParamFreq1].changed = 0;
    phaseInc1 = moduleData->paramData[eParamFreq1].value.asFloat * rho;
  }
  if(moduleData->paramData[eParamFreq2].changed) {
    moduleData->paramData[eParamFreq2].changed = 0;
    phaseInc2 = moduleData->paramData[eParamFreq2].value.asFloat * rho;
  }
  */

  /*
  if(moduleData->paramData[eParamAmp].changed) {
    moduleData->paramData[eParamAmp].changed = 0;
  } 
  */
 
  phase1 += phaseInc1;
  phase2 += phaseInc2;

  while (phase1 > (f32)WAVETABLE_NUM_1) {
    phase1 -= (f32)WAVETABLE_NUM_1;
  }
  while (phase2 > (f32)WAVETABLE_NUM_1) {
    phase2 -= (f32)WAVETABLE_NUM_1;
  }
  
  output = float_to_fr32((wavetable_interp(phase1) + wavetable_interp(phase2)) * 0.125);
  // output = mult_fr1x32x32(output, moduleData->paramData[eParamAmp].value.asFract);

  out0 = output;
  out1 = output;
  out2 = output;
  out3 = output;
}
