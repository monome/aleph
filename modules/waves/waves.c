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
#include "conversion.h"
// bfin
#include "bfin_core.h"
#include "cv.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "module_custom.h"
#include "params.h"
#include "types.h"

// waves
#include "osc_waves.h"
#include "svf_waves.h"
#include "slew.h"

#define WAVES_NVOICES 2

#define WAVES_PM_DEL_SAMPS 0x400
#define WAVES_PM_DEL_SAMPS_1 0x3ff

//-------- data types


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _wavesData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
  // PM delay buffers
  //  fract32 modDelBuf[WAVES_NVOICES][WAVES_PM_DEL_SAMPS];
} wavesData;


// single "voice" structure
typedef struct _waveVoice { 
  // oscillator
  osc osc;
  // filter
  filter_svf svf;
  // osc amp
  fract32 amp;
  // osc output bus
  fract32 oscOut;
  // filter output bus
  fract32 svfOut;

  // amp smoother
  //  filter_1p_lo ampSlew;
  Slew32 ampSlew;
  // cutoff smoother
  //  filter_1p_lo cutSlew;
  Slew32 cutSlew;
  // rq smoother
  //  filter_1p_lo rqSlew;
  Slew32 rqSlew;

  // dry mix
  Slew16 drySlew;
  // wet mix
  Slew16 wetSlew;

  // PM input
  fract32 pmIn;

  // shape mod input
  fract32 wmIn;

  // PM delay buffer
  fract32 modDelBuf[WAVES_PM_DEL_SAMPS];
  //  fract32* modDelBuf;
  // PM delay write index
  u32 modDelWrIdx;
  // PM delay read index
  u32 modDelRdIdx;

} wavesVoice;


wavesVoice voice[WAVES_NVOICES];

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
static const fract32 wavtab[WAVE_SHAPE_NUM][WAVE_TAB_SIZE] = { 
#include "wavtab_data_inc.c" 
};

// additional busses
static fract32 voiceOut[WAVES_NVOICES] = { 0, 0, };


// FIXME: (?)
// using patch points instead of mix points 
static fract32 trash;
static volatile fract32* patch_adc_dac[4][4];
static volatile fract32* patch_osc_dac[2][4];

// 10v cv values (u16, but use fract32 and audio integrators for now)
// static fract32 cvVal[4];
//static filter_1p_lo cvSlew[4];
/* static Slew32 cvSlew[4]; */
/* static u32 cvChan = 0; */


static volatile fract32 cvVal[4];
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

static inline void mix_voice (void) {
  /// can't handle the mix! use pointers for patch points right now
  int i, j;
  volatile fract32** pout = &(patch_osc_dac[0][0]);
  fract32* pin = voiceOut;
  for(i=0; i < 2; i++) {    
    for(j=0; j < 4; j++) {
      **pout = add_fr1x32(**pout, *pin);
      pout++;
    }
    pin++;
  }
}

static inline void mix_adc (void) {
  /// can't handle the mix! use pointers for patch points right now
  int i, j;
  volatile fract32** pout = &(patch_adc_dac[0][0]);
  fract32* pin = in;
  for(i=0; i < 4; i++) {    
    for(j=0; j < 4; j++) {
      **pout = add_fr1x32(**pout, *pin);
      pout++;
    }
    pin++;
  }
}


// frame calculation
static void calc_frame(void) {
  int i;
  wavesVoice* v = voice;
  fract32* vout = voiceOut;

  for(i=0; i<WAVES_NVOICES; i++) {
    // oscillator class includes hz and mod integrators
    v->oscOut = shr_fr1x32( osc_next( &(v->osc) ), 2);

    // /set modulation - FIXME this is redundant...
    osc_pm_in( &(v->osc), v->pmIn );
    osc_wm_in( &(v->osc), v->wmIn );

    // set filter params
    slew32_calc(v->cutSlew);
    slew32_calc(v->rqSlew);
    filter_svf_set_coeff( &(v->svf), v->cutSlew.y );
    filter_svf_set_rq( &(v->svf), v->rqSlew.y );

    // process filter
    v->svfOut = filter_svf_next( &(v->svf), shr_fr1x32(v->oscOut, 1) );

    // process amp/mix smoothing
    slew32_calc(v->ampSlew);
    slew16_calc(v->drySlew);
    slew16_calc(v->wetSlew);

    // mix dry/filter and apply amp
    *vout = mult_fr1x32x32(
				 v->ampSlew.y,
				 add_fr1x32(
					    mult_fr1x32( 
							trunc_fr1x32(v->oscOut), 
							v->drySlew.y
							 ),
					    mult_fr1x32( 
							trunc_fr1x32(v->svfOut), 
							v->wetSlew.y
							 )
					    )
				 );

    
    // advance phase del indices
    v->modDelWrIdx = (v->modDelWrIdx + 1) & WAVES_PM_DEL_SAMPS_1;
    v->modDelRdIdx = (v->modDelRdIdx + 1) & WAVES_PM_DEL_SAMPS_1;
    // set pm input from delay
    v->pmIn = v->modDelBuf[v->modDelRdIdx];    
    // no tricky modulation routing here!
    v->wmIn = v->modDelBuf[v->modDelRdIdx];    
    // advance pointers
    vout++;
    v++;
  } // end voice loop

  // // simple cross-patch modulation
  // add delay, before filter
  voice[0].modDelBuf[voice[0].modDelWrIdx] = voice[1].oscOut;
  voice[1].modDelBuf[voice[1].modDelWrIdx] = voice[0].oscOut;
  /* voice[0].pmIn = voice[1].oscOut; */
  /* voice[1].pmIn = voice[0].oscOut; */

  // zero the outputs
  out[0] = out[1] = out[2] = out[3] = 0;
  
  // patch filtered oscs outputs
  mix_voice();
  
  // patch adc
  mix_adc();
}

//----------------------
//----- external functions

void module_init(void) {
  int i, j;

  // init module/param descriptor
  // intialize local data at start of SDRAM
  data = (wavesData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
  gModuleData = &(data->super);
  strcpy(gModuleData->name, "waves");
  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

  for(i=0; i<WAVES_NVOICES; i++) {
    fract32 tmp = FRACT32_MAX >> 2;
    osc_init( &(voice[i].osc), &wavtab, SAMPLERATE );
    filter_svf_init( &(voice[i].svf) );
    voice[i].amp = tmp;

    slew_init((voice[i].ampSlew), 0, 0, 0 );
    slew_init((voice[i].cutSlew), 0, 0, 0 );
    slew_init((voice[i].rqSlew), 0, 0, 0 );

    slew_init((voice[i].wetSlew), 0, 0, 0 );
    slew_init((voice[i].drySlew), 0, 0, 0 );

    voice[i].modDelWrIdx = 0;
    voice[i].modDelRdIdx = 0;

    //    voice[i].modDelBuf = data->modDelBuf[i];
  }

  for(i=0; i<4; i++) {
    for(j=0; j<4; j++) {
      patch_adc_dac[i][j] = &trash;
    }
  }

  for(i=0; i<2; i++) {
    for(j=0; j<4; j++) {
      patch_osc_dac[i][j] = &trash;
    }
  }

  // cv
  /* slew_init(cvSlew[0] , 0, 0, 0 ); */
  /* slew_init(cvSlew[1] , 0, 0, 0 ); */
  /* slew_init(cvSlew[2] , 0, 0, 0 ); */
  /* slew_init(cvSlew[3] , 0, 0, 0 ); */

  // cv
  filter_1p_lo_init( &(cvSlew[0]), 0xf );
  filter_1p_lo_init( &(cvSlew[1]), 0xf );
  filter_1p_lo_init( &(cvSlew[2]), 0xf );
  filter_1p_lo_init( &(cvSlew[3]), 0xf );



  // set parameters to defaults
  /// slew first
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
 
  param_setup(  eParamCut0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamCut1Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamRq0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamRq1Slew, PARAM_SLEW_DEFAULT );

  param_setup(  eParamWet0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamWet1Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamDry0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamDry1Slew, PARAM_SLEW_DEFAULT );
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
  /* param_setup(  eParamBl1,  	0 ); */
  /* param_setup(  eParamBl0,  	0 ); */

  param_setup(  eParam_cut1,	PARAM_CUT_DEFAULT);
  param_setup(  eParam_rq1,	PARAM_RQ_DEFAULT);
  param_setup(  eParam_mode1,	0);

  param_setup(  eParam_fwet1,	PARAM_AMP_6 );
  param_setup(  eParam_fdry1,	PARAM_AMP_6 );

  param_setup(  eParam_cut0, 	PARAM_CUT_DEFAULT );
  param_setup(  eParam_rq0, 	PARAM_RQ_DEFAULT );
  param_setup(  eParam_mode0,	0);

  param_setup(  eParam_fwet0,	PARAM_AMP_6 );
  param_setup(  eParam_fdry0,	PARAM_AMP_6 );


  param_setup(  eParam_adc0_dac0, 	1 );
  param_setup(  eParam_adc1_dac1,  	1 );
  param_setup(  eParam_adc2_dac2, 	1 );
  param_setup(  eParam_adc3_dac3, 	1 );

  param_setup(  eParam_osc0_dac0, 	1 );
  param_setup(  eParam_osc0_dac1,  	1 );
  param_setup(  eParam_osc0_dac2, 	1 );
  param_setup(  eParam_osc0_dac3, 	1 );
  param_setup(  eParam_osc1_dac0, 	1 );
  param_setup(  eParam_osc1_dac1,  	1 );
  param_setup(  eParam_osc1_dac2, 	1 );
  param_setup(  eParam_osc1_dac3, 	1 );

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
void module_deinit(void) {
  /// why bother, it never happens!
  /* free(amp1Lp); */
  /* free(amp0Lp); */
}


// get number of parameters
extern u32 module_get_num_params(void) {
  return eParamNumParams;
}

// frame callback
void module_process_frame(void) {

  calc_frame();

  // we could shift to fract16 at a cost of only 1b resolution

  /*
  slew32_calc(cvSlew[cvChan]);
  if( !(slew32_sync(cvSlew[cvChan])) ) { 
    cv_update(cvChan, cvSlew[cvChan].y );
  }
  */
  // FIXME: not sure why the macroized slew isn't working here.
  // something to do with access modifiers and the compiler?
  // process the current channel
  // do nothing if the value is stable
  if( filter_1p_sync( &(cvSlew[cvChan]) ) ) {
    ;;
  } else {
    // update the slew filter and store the value
      cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
      // send the value to the cv driver
      cv_update( cvChan, cvVal[cvChan] );
  }

  // update the channel to be processed
  if(++cvChan == 4) {
    cvChan = 0;
  }
}

// lazy inclusion... sorry
#include "param_set.c"
