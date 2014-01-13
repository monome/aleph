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
#include "dac.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "module_custom.h"
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

// static u32 sr;

// oscillators
static osc osc1;
static osc osc2;

// osc output busses
static fract32 oscOut1 = 0;
static fract32 oscOut2 = 0;

// filters
static filter_svf svf1;
static filter_svf svf2;
static fract32 fdry1 = FR32_MAX;
static fract32 fdry2 = FR32_MAX;
static fract32 fwet1 = 0;
static fract32 fwet2 = 0;

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

// dac values (u16, but use fract32 and audio integrators)
static fract32 dacVal[4];
static filter_1p_lo dacSlew[4];
static u8 dacChan = 0;

//----------------------
//----- static function declaration
// frame calculation
static void calc_frame(void);

// initial param set
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

// frame calculation
static void calc_frame(void) {
  fract32 out1, out2;
  
  // osc output
  oscOut1 = shr_fr1x32(osc_next( &(osc1) ), 2);
  oscOut2 = shr_fr1x32(osc_next( &(osc2) ), 2);

  // phase mod feedback with 1frame delay
  osc_pm_in( &osc1, oscOut2 );
  osc_pm_in( &osc2, oscOut1 );

  // shape mod feedback with 1frame delay
  osc_wm_in( &osc1, oscOut2 );
  osc_wm_in( &osc2, oscOut1 );

  ///////////
  ///////////
  // apply filters

  //  svfOut1 = shl_fr1x32(filter_svf_next( &(svf1), shr_fr1x32(oscOut1, 1)), 1);
  //  svfOut2 = shl_fr1x32(filter_svf_next( &(svf2), shr_fr1x32(oscOut2, 1)), 1);
  svfOut1 = filter_svf_next( &(svf1), shr_fr1x32(oscOut1, 1));
  svfOut2 = filter_svf_next( &(svf2), shr_fr1x32(oscOut2, 1));

  /////////
  /////////

  // amp smoothers
  oscAmp1 = filter_1p_lo_next(amp1Lp);
  oscAmp2 = filter_1p_lo_next(amp2Lp);

  // apply osc amplitudes and sum 
  out1 = mult_fr1x32x32(oscAmp1,
			add_fr1x32(mult_fr1x32x32( oscOut1, fdry1),
				   mult_fr1x32x32( svfOut1, fwet1)
				   ));
  
  out2 = mult_fr1x32x32(oscAmp2,
			add_fr1x32(mult_fr1x32x32( oscOut2, fdry2),
				   mult_fr1x32x32( svfOut2, fwet2)
				   ));

  frameVal = add_fr1x32( out1, out2);
  
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


  // filters
  filter_svf_init(&(svf1));
  filter_svf_init(&(svf2));    

  // allocate smoothers
  amp1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp1Lp, oscAmp1 );

  amp2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp2Lp, oscAmp2 );

  // dac
  filter_1p_lo_init( &(dacSlew[0]), 0xf );
  filter_1p_lo_init( &(dacSlew[1]), 0xf );
  filter_1p_lo_init( &(dacSlew[2]), 0xf );
  filter_1p_lo_init( &(dacSlew[3]), 0xf );


  // write descriptors
  /// FIXME: eliminate and move offline !
  fill_param_desc();

  // set parameters to defaults
  param_setup(  eParamFreq1, 	220 << 16 );
  param_setup(  eParamFreq2, 	330 << 16 );
  param_setup(  eParamTune1, 	FIX16_ONE );
  param_setup(  eParamTune2, 	FIX16_ONE );
  param_setup(  eParamWave1, 	0 );
  param_setup(  eParamWave2, 	0 );
  param_setup(  eParamAmp1, 	PARAM_AMP_6 );
  param_setup(  eParamAmp2, 	PARAM_AMP_6 );
  param_setup(  eParamPm12, 	0 );
  param_setup(  eParamPm21, 	0 );
  param_setup(  eParamWm12, 	0 );
  param_setup(  eParamWm21, 	0 );
  param_setup(  eParamBl1,  	0 );
  param_setup(  eParamBl2,  	0 );
  param_setup(  eParam_cut1,	PARAM_CUT_DEFAULT);
  param_setup(  eParam_rq1,	PARAM_RQ_DEFAULT);
  param_setup(  eParam_low1,       PARAM_AMP_6 );
  param_setup(  eParam_high1,	0 );
  param_setup(  eParam_band1,	0 );
  param_setup(  eParam_notch1,	0 );
  param_setup(  eParam_fwet1,	PARAM_AMP_6 );
  param_setup(  eParam_fdry1,	PARAM_AMP_6 );
  param_setup(  eParam_cut2, 	PARAM_CUT_DEFAULT );
  param_setup(  eParam_rq2, 	PARAM_RQ_DEFAULT );
  param_setup(  eParam_low2,	FRACT32_MAX >> 1 );
  param_setup(  eParam_high2,	0 );
  param_setup(  eParam_band2,	0 );
  param_setup(  eParam_notch2,	0 );
  param_setup(  eParam_fwet2,	PARAM_AMP_6 );
  param_setup(  eParam_fdry2,	PARAM_AMP_6 );
  param_setup(  eParamFreq1Smooth, PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamFreq2Smooth, PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamPm12Smooth, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamPm21Smooth, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamWm12Smooth, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamWm21Smooth, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamWave1Smooth, PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamWave2Smooth, PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamAmp1Smooth, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamAmp2Smooth, PARAM_SMOOTH_DEFAULT );
  param_setup(  eParamIoAmp0, 	FRACT32_MAX );
  param_setup(  eParamIoAmp1,  	FRACT32_MAX );
  param_setup(  eParamIoAmp2, 	FRACT32_MAX );
  param_setup(  eParamIoAmp3, 	FRACT32_MAX );
  param_setup(  eParam_dacVal0, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_dacVal1, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_dacVal2, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_dacVal3, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_dacSlew0, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParam_dacSlew1, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParam_dacSlew2, 	PARAM_SMOOTH_DEFAULT );
  param_setup(  eParam_dacSlew3, 	PARAM_SMOOTH_DEFAULT );

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
  
  
  if(dacSlew[dacChan].sync) { ;; } else {
    dacVal[dacChan] = filter_1p_lo_next(&(dacSlew[dacChan]));
    dac_update(dacChan, dacVal[dacChan]);
  }
 
  if(++dacChan == 4) {
    dacChan = 0;
  }
  
}

#include "param_set.c"
