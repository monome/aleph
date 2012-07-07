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
// define twice for preprocesor and for core modules
const u32 kNumParams = MODULE_NUM_PARAMS;

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

  // initialize parameter descriptors
  strcpy(data->paramDesc[eParamAmp].name, "amp");
  strcpy(data->paramDesc[eParamAmp].unit, "amp");
  data->paramDesc[eParamAmp].type = eParamFract;

  strcpy(data->paramDesc[eParamFreq].name, "freq");
  strcpy(data->paramDesc[eParamFreq].unit, "hz");
  data->paramDesc[eParamFreq].type = eParamFloat;

  // initialize parameter values
  data->paramData[eParamAmp].value.asFract = float_to_fr32(0.125f);
  data->paramData[eParamAmp].value.asFloat = 440;

  // initialize wavetable
  init_wavetable();

  // intial phase increment
  phaseInc = rho * 440.f;
}

//------ process frame
void process_frame(void) {
  // check  if freq param changed
  if(data->paramData[eParamFreq].changed) {
    data->paramData[eParamFreq].changed = 0;
    phaseInc = data->paramData[eParamFreq].value.asFloat * rho;
  }
  phase += phaseInc;
  while (phase > (f32)WAVETABLE_NUM_1) {
    phase -= (f32)WAVETABLE_NUM_1;
  }
  output = mult_fr1x32x32(data->paramData[eParamAmp].value.asFract,
			  wavetable_interp(phase));
  out0 = output;
  out1 = output;
  out2 = output;
  out3 = output;
}
