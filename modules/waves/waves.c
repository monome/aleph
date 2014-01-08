/* waves.c
 * aleph-bfin
 * 
 * wavetable synthesis module
 */

// std
#include <math.h>
#include <stdlib.h>
#include <string.h>
// aleph-common
#include "fix.h"
// audio lib
#include "filter_1p.h"
#include "filter_svf.h"
#include "osc.h"
#include "table.h"
#include "conversion.h"
// bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "params.h"
#include "types.h"


//-------- data types


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _wavesData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} wavesData;

//-------------------------
//----- extern vars (initialized here)
moduleData * gModuleData; // module data

//-----------------------
//------ static variables

// pointer to local module data, initialize at top of SDRAM
static wavesData * data;

//-- static allocation (SRAM) for variables that are small and/or frequently accessed:

/// FIXME: wavetables are statically linked constants for now.
/// would like to have them in SDRAM and allow arbitrary asynchronous load.
static const fract32 wavtab[WAVE_TAB_NUM][WAVE_TAB_SIZE] = { 
#include "wavtab_data_inc.c" 
};

static u32 sr;

// oscillators
static osc osc1;
static osc osc2;

// osc output busses
static fract32 oscOut1 = 0;
static fract32 oscOut2 = 0;

// filters
static filter_svf svf1;
static filter_svf svf2;

// filter output busses
static fract32 svfOut1 = 0;
static fract32 svfOut2 = 0;

// osc amplitudes
static fract32  oscAmp1;
static fract32  oscAmp2;

// amp smoothers
static filter_1p_lo* amp1Lp;  
static filter_1p_lo* amp2Lp;

// dry I->O amplitudes
static fract32 ioAmp0;
static fract32 ioAmp1;
static fract32 ioAmp2;
static fract32 ioAmp3;


/// FIXME
static fract32 frameVal;

//----------------------
//----- static function declaration
// frame calculation
static void calc_frame(void);

// frame calculation
static void calc_frame(void) {

  // osc output
  oscOut1 = osc_next( &(osc1) );
  oscOut2 = osc_next( &(osc2) );

  ///// TODO:
  //// no phase mod yet

  ///////////
  ///////////
  // apply filters
  //  tmp = filter_svf_next( &(svf1), osc1);  
  //  tmp = filter_svf_next( &(svf2), osc2);  
  /////////
  /////////

  // amp smoothers
  oscAmp1 = filter_1p_lo_next(amp1Lp);
  oscAmp2 = filter_1p_lo_next(amp2Lp);

  // apply amplitudes and sum 
  ////////////
  //// TEST
#if 0  
  frameVal = add_fr1x32(
			mult_fr1x32x32(oscOut1, oscAmp1),
			mult_fr1x32x32(oscOut2, oscAmp2)
			);
#else
  //////////
  frameVal = oscOut1 >> 2;
  ///////////////
#endif 
  
}

//----------------------
//----- external functions

void module_init(void) {
  u32 i;

  // init module/param descriptor
  // intialize local data at start of SDRAM
  data = (wavesData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
  gModuleData = &(data->super);
  strcpy(gModuleData->name, "aleph-waves");
  gModuleData->paramDesc = data->mParamDesc;
  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

  // fill param descriptor
  fill_param_desc();

  // fill param values with minima as default
  for(i=0; i<eParamNumParams; ++i) {
    gModuleData->paramData[i].value = gModuleData->paramDesc[i].min;
  }

  osc_init( &osc1, &wavtab, SAMPLERATE );
  osc_init( &osc2, &wavtab, SAMPLERATE );

  oscAmp1 = oscAmp2 = INT32_MAX >> 2;

  osc_set_hz( &osc1, fix16_from_int(220) );
  osc_set_hz( &osc2, fix16_from_int(330) );

  ioAmp0 = FR32_MAX;
  ioAmp1 = FR32_MAX;
  ioAmp2 = FR32_MAX;
  ioAmp3 = FR32_MAX;

  // initial param values
  /// ok, for now
  gModuleData->paramData[eParamFreq1 ].value = osc1.hz ;
  gModuleData->paramData[eParamFreq2 ].value = osc2.hz ;

  gModuleData->paramData[eParamAmp1].value = oscAmp1;
  gModuleData->paramData[eParamAmp2].value = oscAmp2;

  gModuleData->paramData[eParamIoAmp0 ].value = ioAmp0;
  gModuleData->paramData[eParamIoAmp1 ].value = ioAmp1;
  gModuleData->paramData[eParamIoAmp2 ].value = ioAmp2;
  gModuleData->paramData[eParamIoAmp3 ].value = ioAmp3;

  // filters
  filter_svf_init(&(svf1));
  filter_svf_init(&(svf2));    

  filter_svf_set_rq(&(svf1), 0x1000);
  filter_svf_set_low(&(svf1), 0x4000);
  filter_svf_set_coeff(&(svf1), 0x5ff00000 );
    
  filter_svf_set_rq(&(svf2), 0x1000);
  filter_svf_set_low(&(svf2), 0x4000);
  filter_svf_set_coeff(&(svf2), 0x4ff00000 );
    
  // allocate smoothers
  amp1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp1Lp, oscAmp1 );

  amp2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp2Lp, oscAmp2 );
}

// de-init
void module_deinit(void) {
  free(amp1Lp);
  free(amp2Lp);
}


// get number of parameters
extern u32 module_get_num_params(void) {
  return eParamNumParams;
}

// frame callback
void module_process_frame(void) {

  calc_frame();
  out[0] = add_fr1x32(frameVal, mult_fr1x32x32(in[0], ioAmp0));
  out[1] = add_fr1x32(frameVal, mult_fr1x32x32(in[1], ioAmp1));
  out[2] = add_fr1x32(frameVal, mult_fr1x32x32(in[2], ioAmp2));
  out[3] = add_fr1x32(frameVal, mult_fr1x32x32(in[3], ioAmp3));
}

#include "param_set.c"
