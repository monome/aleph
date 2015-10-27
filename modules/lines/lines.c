/* lines.c
   aleph-audio

   multichannel, routable delay lines + filters.
   suitable for echoes, loopers, etc
*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"


#include "bfin_core.h"
#include "cv.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio
#include "buffer.h"
#include "filter_svf.h"
#include "filter_1p.h"
#include "filter_ramp.h"

#include "delayFadeN.h"
#include "module.h"
////test
#include "noise.h"
/// lines
#include "params.h"

#define TEST 0

//-----------------------
//------ static variables

// total SDRAM is 64M
// each line 60ish seconds for now
#define LINES_BUF_FRAMES 0x2bf200
// try...
//#define LINES_BUF_FRAMES 0x600000
//#define LINES_BUF_FRAMES 0x1000000
//#define LINES_BUF_FRAMES 0xbb8000 // 256 seconds @ 48k
#define NLINES 2


// data structure of external memory
typedef struct _linesData {
  ModuleData super;
  //  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
  volatile fract32 audioBuffer[NLINES][LINES_BUF_FRAMES];
} linesData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData; 


//-----------------------
//------ static variables

// pointer to all external memory
linesData* pLinesData;

// delay lines (each has buffer descriptor and read/write taps)
delayFadeN lines[NLINES];

// state variable filters
filter_svf svf[NLINES];

//---- mix points
// each input -> one delay
fract32 mix_adc_del[4][2] = { {0, 0}, {0, 0}, {0, 0}, {0, 0} };
// each delay -> each delay
fract32 mix_del_del[2][2] = { { 0, 0 }, { 0, 0 } };
// each input -> each output
fract32 mix_adc_dac[4][4] = { { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 } };
// each delay -> each dac
fract32 mix_del_dac[2][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

// svf balance
fract32 mix_fwet[NLINES] = { 0, 0 };
fract32 mix_fdry[NLINES] = { 0, 0 };

// -- mixed inputs
// delay 
fract32 in_del[NLINES] = { 0, 0 };

//-- outputs
// delay
fract32 out_del[NLINES] = { 0, 0 };
fract32 out_svf[NLINES] = { 0, 0 };

//-- parameter integrators
filter_1p_lo svfCutSlew[2];
filter_1p_lo svfRqSlew[2];

//--- crossfade stuff
/// which tap we are fading towards...
u8 fadeTargetRd[2] = { 0, 0 };
u8 fadeTargetWr[2] = { 0, 0 };

// crossfade integrators
/* filter_1p_lo lpFadeRd[2]; */
/* filter_1p_lo lpFadeWr[2]; */
filter_ramp_tog lpFadeRd[2];
filter_ramp_tog lpFadeWr[2];

// 10v dac values (u16, but use fract32 and audio integrators, for now)
fract32 cvVal[4];
filter_1p_lo cvSlew[4];
u8 cvChan = 0;

///////////////
// try this out
/// if set, param changes triggering fade will be ignored if a fade is in progress.
static const u8 fadeIgnore = 1;
////////////////

// initial param set
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


// mix delay inputs
static void mix_del_inputs(void) {
  //  u8 i, j;
  //  fract32* pIn;
  fract32 mul;
  
  //--- del 0
  in_del[0] = 0;

  //adc->del
  mul = mix_adc_del[0][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_del[1][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_del[2][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_del[3][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[3], mul)); 

  // del->del
  mul = mix_del_del[0][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_del[1][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(out_del[1], mul)); 

  //--- del 1
  in_del[1] = 0;
  // adc
  mul = mix_adc_del[0][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_del[1][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_del[2][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_del[3][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[3], mul)); 
  // del 
  mul = mix_del_del[0][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_del[1][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(out_del[1], mul));
}

static void mix_outputs(void) {
  fract32 mul;
  
  //-- out 0
  out[0] = 0;
  // del
  mul = mix_del_dac[0][0];
  out[0] = add_fr1x32(out[0], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_dac[1][0];
  out[0] = add_fr1x32(out[0], mult_fr1x32x32(out_del[1], mul)); 
  // adc
  mul = mix_adc_dac[0][0];
  out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_dac[1][0];
  out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_dac[2][0];
  out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_dac[3][0];
  out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[3], mul)); 

  //-- out 1
  out[1] = 0;
  // del
  mul = mix_del_dac[0][1];
  out[1] = add_fr1x32(out[1], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_dac[1][1];
  out[1] = add_fr1x32(out[1], mult_fr1x32x32(out_del[1], mul)); 
  // adc
  mul = mix_adc_dac[0][1];
  out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_dac[1][1];
  out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_dac[2][1];
  out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_dac[3][1];
  out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[3], mul)); 

  //-- out 2
  out[2] = 0;
  // del
  mul = mix_del_dac[0][2];
  out[2] = add_fr1x32(out[2], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_dac[1][2];
  out[2] = add_fr1x32(out[2], mult_fr1x32x32(out_del[1], mul)); 
  // adc
  mul = mix_adc_dac[0][2];
  out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_dac[1][2];
  out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_dac[2][2];
  out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_dac[3][2];
  out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[3], mul)); 

  //-- out 3
  out[3] = 0;
  // del
  mul = mix_del_dac[0][3];
  out[3] = add_fr1x32(out[3], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_dac[1][3];
  out[3] = add_fr1x32(out[3], mult_fr1x32x32(out_del[1], mul)); 
  // adc
  mul = mix_adc_dac[0][3];
  out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_dac[1][3];
  out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_dac[2][3];
  out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_dac[3][3];
  out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[3], mul));

}

//----------------------
//----- external functions

void module_init(void) {
  u8 i;
  u32 j;
  // init module/params
  pLinesData = (linesData*)SDRAM_ADDRESS;
  
  gModuleData = &(pLinesData->super);
  strcpy(gModuleData->name, "lines");

  gModuleData->paramData = (ParamData*)pLinesData->mParamData;
  gModuleData->numParams = eParamNumParams;

  for(i=0; i<NLINES; i++) {
    delayFadeN_init(&(lines[i]), pLinesData->audioBuffer[i], LINES_BUF_FRAMES);
    filter_svf_init(&(svf[i]));

    filter_1p_lo_init(&(svfCutSlew[i]), 0x3fffffff);
    filter_1p_lo_init(&(svfRqSlew[i]), 0x3fffffff);

    filter_ramp_tog_init(&(lpFadeRd[i]), 0);
    filter_ramp_tog_init(&(lpFadeWr[i]), 0);


    /* for(j=0; j<LINES_BUF_FRAMES; ++j) { */
    /*   pLinesData->audioBuffer[i][j] = 0; */
    /* } */

    // need to zero everything to avoid horrible noise at boot...
    memset(pLinesData->audioBuffer[i], 0, LINES_BUF_FRAMES * sizeof(fract32));
    // however, it is causing crashes or hangs here, for some damn reason.

    // at least zero the end of the buffer
    /* for(j=LINES_BUF_FRAMES - 100000; j < LINES_BUF_FRAMES - 1; ++j) { */
    /*   pLinesData->audioBuffer[i][j] = 0x00000000;  */
    /* } */
  }

  // dac
  filter_1p_lo_init( &(cvSlew[0]), 0xf );
  filter_1p_lo_init( &(cvSlew[1]), 0xf );
  filter_1p_lo_init( &(cvSlew[2]), 0xf );
  filter_1p_lo_init( &(cvSlew[3]), 0xf );

  /// setup params with intial values

  param_setup( eParamFade0 , 0x100000 );
  param_setup( eParamFade1 , 0x100000 );

  param_setup( 	eParam_loop0,		PARAM_SECONDS_MAX );
  param_setup( 	eParam_rMul0,		0x10000 );
  param_setup( 	eParam_rDiv0,		0x10000 );
  param_setup( 	eParam_write0,		FRACT32_MAX );
  param_setup( 	eParam_pre0,		0 );
  param_setup( 	eParam_pos_write0,		0 );
  param_setup( 	eParam_pos_read0,		0 );

  param_setup( 	eParam_delay0,		0x4000 );

  param_setup( 	eParam_run_read0, 1 );
  param_setup( 	eParam_run_write0, 1 );

  param_setup( 	eParam_loop1,		PARAM_SECONDS_MAX );
  param_setup( 	eParam_rMul1,		0x10000 );
  param_setup( 	eParam_rDiv1,		0x10000 );
  param_setup( 	eParam_write1,		FRACT32_MAX );
  param_setup( 	eParam_pre1,		0 );
  param_setup( 	eParam_pos_write1,		0 );
  param_setup( 	eParam_pos_read1,		0 );

  param_setup( 	eParam_delay1,		0x4000 );

  param_setup( 	eParam_run_read1, 1 );
  param_setup( 	eParam_run_write1, 1 );

  param_setup( 	eParam_del1_dac0,		PARAM_AMP_12 );
  param_setup( 	eParam_del1_dac1,		PARAM_AMP_6 );
  param_setup( 	eParam_del1_dac2,		PARAM_AMP_12 );
  param_setup( 	eParam_del1_dac3,		PARAM_AMP_6 );

  param_setup( 	eParam_adc0_dac0,		PARAM_AMP_12 );
  param_setup( 	eParam_adc0_dac1,		PARAM_AMP_12 );
  param_setup( 	eParam_adc0_dac2,		PARAM_AMP_12 );
  param_setup( 	eParam_adc0_dac3,		PARAM_AMP_12 );
  param_setup( 	eParam_adc1_dac0,		PARAM_AMP_12 );
  param_setup( 	eParam_adc1_dac1,		PARAM_AMP_12 );
  param_setup( 	eParam_adc1_dac2,		PARAM_AMP_12 );
  param_setup( 	eParam_adc1_dac3,		PARAM_AMP_12 );
  param_setup( 	eParam_adc2_dac0,		PARAM_AMP_12 );
  param_setup( 	eParam_adc2_dac1,		PARAM_AMP_12 );
  param_setup( 	eParam_adc2_dac2,		PARAM_AMP_12 );
  param_setup( 	eParam_adc2_dac3,		PARAM_AMP_12 );
  param_setup( 	eParam_adc3_dac0,		PARAM_AMP_12 );
  param_setup( 	eParam_adc3_dac1,		PARAM_AMP_12 );
  param_setup( 	eParam_adc3_dac2,		PARAM_AMP_12 );
  param_setup( 	eParam_adc3_dac3,		PARAM_AMP_12 );


  param_setup(  eParam_freq1,	PARAM_CUT_DEFAULT);
  param_setup(  eParam_rq1,	PARAM_RQ_DEFAULT);
  param_setup(  eParam_low1,       PARAM_AMP_6 );
  param_setup(  eParam_high1,	0 );
  param_setup(  eParam_band1,	0 );
  param_setup(  eParam_notch1,	0 );
  param_setup(  eParam_fwet1,	PARAM_AMP_6 );
  param_setup(  eParam_fdry1,	PARAM_AMP_6 );

  param_setup(  eParam_freq0, 	PARAM_CUT_DEFAULT );
  param_setup(  eParam_rq0, 	PARAM_RQ_DEFAULT );
  param_setup(  eParam_low0,	FRACT32_MAX >> 1 );
  param_setup(  eParam_high0,	0 );
  param_setup(  eParam_band0,	0 );
  param_setup(  eParam_notch0,	0 );
  param_setup(  eParam_fwet0,	PARAM_AMP_6 );
  param_setup(  eParam_fdry0,	PARAM_AMP_6 );

  param_setup(  eParamCut0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamCut1Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamRq0Slew, PARAM_SLEW_DEFAULT );
  param_setup(  eParamRq1Slew, PARAM_SLEW_DEFAULT );


  param_setup(  eParam_cvSlew0, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_cvSlew1, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_cvSlew2, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_cvSlew3, PARAM_SLEW_DEFAULT );

  param_setup(  eParam_cvVal0, PARAM_CV_VAL_DEFAULT );
  param_setup(  eParam_cvVal1, PARAM_CV_VAL_DEFAULT );
  param_setup(  eParam_cvVal2, PARAM_CV_VAL_DEFAULT );
  param_setup(  eParam_cvVal3, PARAM_CV_VAL_DEFAULT );

}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}


void module_process_frame(void) { 
  static fract32 tmpDel, tmpSvf;
  u8 i;

  tmpDel = 0;
  tmpSvf = 0;

  // mix inputs to delay lines
  mix_del_inputs();

  /// TEST

  for(i=0; i<NLINES; i++) {
    // process fade integrator
    //    lines[i].fadeWr = filter_ramp_tog_next(&(lpFadeWr[i]));
    lines[i].fadeRd = filter_ramp_tog_next(&(lpFadeRd[i]));

    // process delay line
    tmpDel = delayFadeN_next( &(lines[i]), in_del[i]);	    
    // process filters
    // check integrators for filter params

    if( !filter_1p_sync(&(svfCutSlew[i])) ) {
      filter_svf_set_coeff( &(svf[i]), filter_1p_lo_next(&(svfCutSlew[i])) );
    }
    if( !filter_1p_sync(&(svfRqSlew[i])) ) {
      filter_svf_set_rq( &(svf[i]), filter_1p_lo_next(&(svfRqSlew[i])) );
    }
    tmpSvf = filter_svf_next( &(svf[i]), tmpDel);  
    // mix
    tmpDel = mult_fr1x32x32( tmpDel, mix_fdry[i] );
    tmpDel = add_fr1x32(tmpDel, mult_fr1x32x32(tmpSvf, mix_fwet[i]) );

    out_del[i] = tmpDel;

  } // end lines loop 

  // mix outputs to DACs
  /// TEST
  /* out[0] = in[0]; */
  /* out[1] = in[1]; */
  /* out[2] = in[2]; */
  /* out[3] = in[3]; */
  out[0] = out[1] = out[2] = out[3] = 0x00000000;
  mix_outputs();

  /// do CV output
  if( !filter_1p_sync(&(cvSlew[cvChan])) ) {
    cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
    cv_update(cvChan, cvVal[cvChan]);
  }
  
  if(++cvChan == 4) {
    cvChan = 0;
  }
}

// parameter set function
// (a very long switch statement)
#include "param_set.c"

