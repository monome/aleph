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
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} wavesData;

//-------------------------
//----- extern vars (initialized here)
ModuleData * gModuleData; // module data
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
static osc osc0;

// osc output busses
static fract32 oscOut1 = 0;
static fract32 oscOut0 = 0;

// filters
static filter_svf svf1;
static filter_svf svf0;
static fract32 fdry1 = FR32_MAX;
static fract32 fdry0 = FR32_MAX;
static fract32 fwet1 = 0;
static fract32 fwet0 = 0;

// filter output busses
static fract32 svfOut1 = 0;
static fract32 svfOut0 = 0;

// osc amplitudes
static fract32  oscAmp1;
static fract32  oscAmp0;

// amp smoothers
static filter_1p_lo amp1Lp;  
static filter_1p_lo amp0Lp;

// dry I->O amplitudes
static fract32 ioAmp0; 
static fract32 ioAmp1; 
static fract32 ioAmp2;
static fract32 ioAmp3;

/// mixes
// each input -> each output
static fract32 mix_adc_dac[4][4] = { { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 } };


// each osc -> each output
static fract32 mix_osc_dac[2][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };


/// FIXME
static fract32 frameVal;

// 10v dac values (u16, but use fract32 and audio integrators, for now)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

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
  //  fract32 out1, out0;
  
  // osc output
  oscOut1 = shr_fr1x32(osc_next( &(osc1) ), 2);
  oscOut0 = shr_fr1x32(osc_next( &(osc0) ), 2);

  // phase mod feedback with 1frame delay
  osc_pm_in( &osc1, oscOut0 );
  osc_pm_in( &osc0, oscOut1 );

  // shape mod feedback with 1frame delay
  osc_wm_in( &osc1, oscOut0 );
  osc_wm_in( &osc0, oscOut1 );

  ///////////
  ///////////
  // apply filters

  //  svfOut1 = shl_fr1x32(filter_svf_next( &(svf1), shr_fr1x32(oscOut1, 1)), 1);
  //  svfOut2 = shl_fr1x32(filter_svf_next( &(svf2), shr_fr1x32(oscOut2, 1)), 1);
  svfOut1 = filter_svf_next( &(svf1), shr_fr1x32(oscOut1, 1));
  svfOut0 = filter_svf_next( &(svf0), shr_fr1x32(oscOut0, 1));

  /////////
  /////////

  // amp smoothers
  oscAmp1 = filter_1p_lo_next(&amp1Lp);
  oscAmp0 = filter_1p_lo_next(&amp0Lp);

  // apply osc amplitudes and sum 
  oscOut1 = mult_fr1x32x32(oscAmp1,
			add_fr1x32(mult_fr1x32x32( oscOut1, fdry1),
				   mult_fr1x32x32( svfOut1, fwet1)
				   ));
  
  oscOut0 = mult_fr1x32x32(oscAmp0,
			add_fr1x32(mult_fr1x32x32( oscOut0, fdry0),
				   mult_fr1x32x32( svfOut0, fwet0)
				   ));

  ////
  /// fixme: mono
  frameVal = add_fr1x32( oscOut0, oscOut1);

  // mix to output
  //...todo
  
}

//----------------------
//----- external functions

void module_init(void) {

  // init module/param descriptor
  // intialize local data at start of SDRAM
  data = (wavesData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
  gModuleData = &(data->super);
  strcpy(gModuleData->name, "aleph-waves");
  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

  osc_init( &osc1, &wavtab, SAMPLERATE );
  osc_init( &osc0, &wavtab, SAMPLERATE );

  oscAmp1 = oscAmp0 = INT32_MAX >> 2;

  // filters
  filter_svf_init(&(svf1));
  filter_svf_init(&(svf0));    

  // allocate smoothers
  filter_1p_lo_init( &amp1Lp, oscAmp1 );
  filter_1p_lo_init( &amp0Lp, oscAmp0 );

  // dac
  filter_1p_lo_init( &(cvSlew[0]), 0xf );
  filter_1p_lo_init( &(cvSlew[1]), 0xf );
  filter_1p_lo_init( &(cvSlew[2]), 0xf );
  filter_1p_lo_init( &(cvSlew[3]), 0xf );


  // write descriptors
  /// FIXME: eliminate and move offline !
#if 0
  fill_param_desc();
#endif

  // set parameters to defaults
  param_setup(  eParamHz1, 	220 << 16 );
  param_setup(  eParamHz0, 	330 << 16 );
  param_setup(  eParamTune1, 	FIX16_ONE );
  param_setup(  eParamTune0, 	FIX16_ONE );
  param_setup(  eParamWave1, 	0 );
  param_setup(  eParamWave0, 	0 );
  param_setup(  eParamAmp1, 	PARAM_AMP_6 );
  param_setup(  eParamAmp0, 	PARAM_AMP_6 );
  param_setup(  eParamPm10, 	0 );
  param_setup(  eParamPm01, 	0 );
  param_setup(  eParamWm10, 	0 );
  param_setup(  eParamWm01, 	0 );
  param_setup(  eParamBl1,  	0 );
  param_setup(  eParamBl0,  	0 );
  param_setup(  eParam_cut1,	PARAM_CUT_DEFAULT);
  param_setup(  eParam_rq1,	PARAM_RQ_DEFAULT);
  param_setup(  eParam_low1,       PARAM_AMP_6 );
  param_setup(  eParam_high1,	0 );
  param_setup(  eParam_band1,	0 );
  param_setup(  eParam_notch1,	0 );
  param_setup(  eParam_fwet1,	PARAM_AMP_6 );
  param_setup(  eParam_fdry1,	PARAM_AMP_6 );
  param_setup(  eParam_cut0, 	PARAM_CUT_DEFAULT );
  param_setup(  eParam_rq0, 	PARAM_RQ_DEFAULT );
  param_setup(  eParam_low0,	FRACT32_MAX >> 1 );
  param_setup(  eParam_high0,	0 );
  param_setup(  eParam_band0,	0 );
  param_setup(  eParam_notch0,	0 );
  param_setup(  eParam_fwet0,	PARAM_AMP_6 );
  param_setup(  eParam_fdry0,	PARAM_AMP_6 );
  param_setup(  eParamHz1Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamHz0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamPm10Slew, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParamPm01Slew, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParamWm10Slew, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParamWm01Slew, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParamWave1Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamWave0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamAmp1Slew, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParamAmp0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_adc0_dac0, 	FRACT32_MAX );
  param_setup(  eParam_adc1_dac1,  	FRACT32_MAX );
  param_setup(  eParam_adc2_dac2, 	FRACT32_MAX );
  param_setup(  eParam_adc3_dac3, 	FRACT32_MAX );
  param_setup(  eParam_cvVal0, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_cvVal1, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_cvVal2, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_cvVal3, 	FRACT32_MAX >> 1 );
  param_setup(  eParam_cvSlew0, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParam_cvSlew1, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParam_cvSlew2, 	PARAM_SLEW_DEFAULT );
  param_setup(  eParam_cvSlew3, 	PARAM_SLEW_DEFAULT );

  

}

// de-init
/*   /// why bother, it never happens */
/* void module_deinit(void) { */
/* } */


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
  
  if(cvSlew[cvChan].sync) { ;; } else {
    cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
    dac_update(cvChan, cvVal[cvChan]);
  }
 
  if(++cvChan == 4) {
    cvChan = 0;
  }
  
}

#include "param_set.c"
