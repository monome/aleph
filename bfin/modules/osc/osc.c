#include <fract_math.h>
#include <fract2float_conv.h>
#include <math.h>
#include <string.h>

//aleph-common
#include "fix.h"
#include "param_common.h"

// aleph-bfin
#include "bfin_core.h"
#include "module.h"
#include "osc.h"

// external data, manually located at start of SDRAM
oscData_t * data = (oscData_t *)SDRAM_ADDRESS;
// pointer to data superclass (for access by bfin core routines)
moduleData_t* moduleData;
static u8 butstate = 0;

//---------------------------
//---- static vars
// current phase
static fix16_t phase = 0.f;
// constant representing phase change for 1hz
static fract32 rho;
// phase increment
static fix16_t phaseInc;
// output value
static fract32 output = 0;
// button toggle status
// static u8 butstate = 0;
static u32 numOscs;
// frame counter
static u64 framecount = 0;
// max phase
static fix16_t maxPhase;

//---------------------------
//----- static functions
static void init_wavetable(void) {
  const f32 inc = M_TWOPI / (f32)WAVETABLE_NUM;
  f32 x = 0.f;
  u16 i;
  for (i=0; i<WAVETABLE_NUM; i++) {
    data->sineTable[i] = float_to_fr32(sinf(x));
    x += inc;
  }
}

// linear-interpolated lookup
static fract32 wavetable_interp(fix16_t phase) {
  static u16 x;
  static u16 xnext;
  static fract32 fx;
  static fract32 fxnext;

  x = FIX16_TO_U16(phase);
  xnext = x + 1;

  //  fxnext = FIX16_FRACT_TRUNC(phase);
  // lose one bit in unsigned->signed
  fxnext = ((fract32)(phase & 0xffff)) << 15;
  fx = sub_fr1x32(0x7fffffff, fxnext);
  
  if ( xnext > WAVETABLE_NUM_1) {
    xnext -= WAVETABLE_NUM_1;
  }
  
  return add_fr1x32(
		    mult_fr1x32x32(data->sineTable[x], fx),
		    mult_fr1x32x32(data->sineTable[xnext], fxnext)
		    );
}


//---------------------------
//----- external functions


//----- init
void module_init(void) {
  //  fract32 dum;
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

  // max phase in fix16
  maxPhase = fix16_from_int(WAVETABLE_NUM_1);
  // rho constant
  rho = float_to_fr32( (f32)WAVETABLE_NUM / (f32)SAMPLERATE );

  // initialize wavetable
  init_wavetable();

  // intial phase increment
  phaseInc = fix16_mul(rho >> 16, fix16_from_float(220.f));
  // number of re-calculations
  numOscs = 1;
}

//------ process frame
void module_process_frame(void) {
  u32 i;

  // check  if freq param changed
  if(moduleData->paramData[eParamFreq].changed) {
    moduleData->paramData[eParamFreq].changed = 0;
    //    phaseInc = moduleData->paramData[eParamFreq].value.asFloat * rho;
    phaseInc =  fix16_mul(rho >> 16, fix16_from_float(moduleData->paramData[eParamFreq].value.asFloat));
  }    

  phase = fix16_add(phase, phaseInc);
  while(phase > maxPhase) { phase = fix16_sub(phase, maxPhase); }

  // recalculate numOscs times
  for(i=0; i<numOscs; i++) {
    fract32 o;

    o = mult_fr1x32x32(moduleData->paramData[eParamAmp].value.asFract,
		       wavetable_interp(phase));
    output = o;
  }
  

  out0 = output;
  out1 = output;
  out2 = output;
  out3 = output;

  framecount++;
  if(framecount > SAMPLERATE) {
    framecount = 0;
    // numOscs++;
  }
}

//----- handle button
void module_handle_button(const u16 state) {
  
  static f32 freq;
  freq = moduleData->paramData[eParamFreq].value.asFloat;
  if(butstate) { butstate = 0; } else { butstate = 1; }
  freq *= 1.25;
  while (freq > 2000) { freq *= 0.5; }
  moduleData->paramData[eParamFreq].value.asFloat = freq;
  moduleData->paramData[eParamFreq].changed = 1;
  
}

//----- update LEDs
u8 module_update_leds(void) {
  /*
  static u8 leds;
  static fract32 outLevel;
  outLevel = max_fr1x32(
			max_fr1x32(
				   max_fr1x32(
					      abs_fr1x32(out0),
					      abs_fr1x32(out1)
					      ),
				   abs_fr1x32(out2)
				   ),
			abs_fr1x32(out3)
			);
  
  if (outLevel > 1076291388)      { leds = 0x3f; }
  else if (outLevel > 539423503)  { leds = 0x3e; }
  else if (outLevel > 135497058)  { leds = 0x3c; }
  else if (outLevel > 53942350)   { leds = 0x38; }
  else if (outLevel > 8549286)    { leds = 0x30; }
  else if (outLevel > 2147484)    { leds = 0x20; }
  
  return leds;
  */
  //  return numOscs & 0x3f;
  return 0x3f;
}
