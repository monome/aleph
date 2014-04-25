/* dacs.c
   aleph-bfin

   simplest CV output module.

*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"

// aleph-bfin
#include "bfin_core.h"
#include "dac.h"
#include "gpio.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio
#include "filter_1p.h"
#include "module.h"
#include "pitch_shift.h"

/// custom
#include "params.h"


// total SDRAM is 64M
// each line 16 bit address
#define LINES_BUF_FRAMES 0x10000
//#define LINES_BUF_FRAMES 0xFFFFFF
// try...
//#define LINES_BUF_FRAMES 0x600000
//#define LINES_BUF_FRAMES 0x1000000
//#define LINES_BUF_FRAMES 0xbb8000 // 256 seconds @ 48k
#define NLINES 1
#define PARAM_SECONDS_MAX 0x003c0000

delayLine lines[NLINES];

ParamValue auxL[4];
ParamValue auxR[4];
ParamValue auxLTarget[4];
ParamValue auxRTarget[4];
#define AUX_DEFAULT PARAM_AMP_0

ParamValue pan[4];
ParamValue panTarget[4];
#define PAN_DEFAULT PAN_MAX/2

ParamValue fader[4];
ParamValue faderTarget[4];
#define FADER_DEFAULT PARAM_AMP_0

ParamValue effect[4];
ParamValue effectTarget[4];
#define EFFECT_DEFAULT PARAM_AMP_0
//ParamValue eq_hi[4];
//ParamValue eq_mid[4];
//ParamValue eq_lo[4];

ParamValue delayTime=0;
ParamValue delayTimeTarget=0;
static filter_1p_lo delayTimeSlew;

ParamValue feedback=0;
ParamValue feedbackTarget=0;

// data structure of external memory
typedef struct _dacsData {
  ModuleData super;
  //ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
  volatile fract32 audioBuffer[NLINES][LINES_BUF_FRAMES];
} dacsData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;
//-----------------------
//------ static variables

// pointer to all external memory
dacsData* pDacsData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 dacVal[4];
static filter_1p_lo dacSlew[4];
static u8 dacChan = 0;

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}
void module_init(void) {


  // init module/param descriptor
  pDacsData = (dacsData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "aleph-dacs");

  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;


  filter_1p_lo_init( &(dacSlew[0]), 0 );
  filter_1p_lo_init( &(dacSlew[1]), 0 );
  filter_1p_lo_init( &(dacSlew[2]), 0 );
  filter_1p_lo_init( &(dacSlew[3]), 0 );

  param_setup(  eParam_slew0, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_slew1, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_slew2, PARAM_SLEW_DEFAULT );
  param_setup(  eParam_slew3, PARAM_SLEW_DEFAULT );

  param_setup(  eParam_dac0, PARAM_CV_VAL_DEFAULT );
  param_setup(  eParam_dac1, PARAM_CV_VAL_DEFAULT );
  param_setup(  eParam_dac2, PARAM_CV_VAL_DEFAULT );
  param_setup(  eParam_dac3, PARAM_CV_VAL_DEFAULT );

  param_setup( 	eParam_auxL0,		AUX_DEFAULT );
  param_setup( 	eParam_auxR0,		AUX_DEFAULT );
  param_setup( 	eParam_pan0,		PAN_DEFAULT );
  param_setup( 	eParam_fader0,		FADER_DEFAULT );
  param_setup( 	eParam_effect0,		EFFECT_DEFAULT );

  param_setup( 	eParam_auxL1,		AUX_DEFAULT );
  param_setup( 	eParam_auxR1,		AUX_DEFAULT );
  param_setup( 	eParam_pan1,		PAN_DEFAULT );
  param_setup( 	eParam_fader1,		FADER_DEFAULT );
  param_setup( 	eParam_effect1,		EFFECT_DEFAULT );

  param_setup( 	eParam_auxL2,		AUX_DEFAULT );
  param_setup( 	eParam_auxR2,		AUX_DEFAULT );
  param_setup( 	eParam_pan2,		PAN_DEFAULT );
  param_setup( 	eParam_fader2,		FADER_DEFAULT );
  param_setup( 	eParam_effect2,		EFFECT_DEFAULT );

  param_setup( 	eParam_auxL3,		AUX_DEFAULT );
  param_setup( 	eParam_auxR3,		AUX_DEFAULT );
  param_setup( 	eParam_pan3,		PAN_DEFAULT );
  param_setup( 	eParam_fader3,		FADER_DEFAULT );
  param_setup( 	eParam_effect3,		EFFECT_DEFAULT );


  delay_init(&(lines[0]), pDacsData->audioBuffer[0], LINES_BUF_FRAMES);

  filter_1p_lo_init( &delayTimeSlew, 0 );

  //param_setup( 	eParam_delay0,		0 );
  param_setup( 	eParam_feedback0,		FADER_DEFAULT );
  param_setup( 	eParam_feedback0,		0 );

  //delay_set_loop_samp(&(lines[0]), LINES_BUF_FRAMES/2);
  //delay_set_run_write(&(lines[0]), 1);
  //delay_set_run_read(&(lines[0]), 1);
  //delay_set_pos_write_samp(&(lines[0]), 0);
  //delay_set_pos_read_samp(&(lines[0]), 0);
  //delay_set_delay_samp(&(lines[0]), delayTimeTarget/256);
}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}


/// fixme: don't really need these
//static u8 dacDirty[4] = { 0, 0, 0, 0};
//// FIXME:
/* for now, stagger DAC channels across consecutive audio frames
   better method might be:
   - enable DMA tx interrupt
   - each ISR calls the next channel to be loaded
   - last thing audio ISR does is call the first DAC channel to be loaded
   - dac_update writes to 4x16 volatile buffer
*/
//static u8 dacChan = 0;
//

void mix_aux_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) ;

void mix_panned_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) ;

fract32 delayOutput = 0, delayInput = 0;
void module_process_frame(void) {

  //Update one of the CV outputs
  if(dacSlew[dacChan].sync) { ;; } else {
    dacVal[dacChan] = filter_1p_lo_next(&(dacSlew[dacChan]));
    dac_update(dacChan, shr_fr1x32(dacVal[dacChan], 15) & DAC_VALUE_MASK);
  }

  //Queue up the next CV output for processing next audio frame
  if(++dacChan == 4) {
    dacChan = 0;
  }

  //Zero the audio dacs, before mixing
  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;

  u8 i;
  //hacky slew!!!
  for (i=0;i<4;i++) {
      auxL[i] = auxLTarget[i]/100*1+auxL[i]/100*99;
      auxR[i] = auxRTarget[i]/100*1+auxR[i]/100*99;
      pan[i] = panTarget[i]/100*1+pan[i]/100*99;
      fader[i] = faderTarget[i]/100*1+fader[i]/100*99;
      effect[i] = effectTarget[i]/100*1+effect[i]/100*99;
  }
  feedback = feedbackTarget/100*1+feedback/100*99;
  //delayTime = delayTimeTarget/256*1+delayTime/256*99;
  if(delayTimeSlew.sync) { ;; } else {
    delayTime = filter_1p_lo_next(&delayTimeSlew);

    //update delay time
    //delay_set_delay_24_8(&(lines[0]), delayTime);
    //delay_set_delay_samp(&(lines[0]), delayTimeTarget);
    //delay_set_rate(&(lines[0]), delayTimeTarget / 100);
  }

  mix_panned_mono(in[0], &(out[1]), &(out[0]), pan[0], fader[0]);
  mix_panned_mono(in[1], &(out[1]), &(out[0]), pan[1], fader[1]);
  mix_panned_mono(in[2], &(out[1]), &(out[0]), pan[2], fader[2]);
  mix_panned_mono(in[3], &(out[1]), &(out[0]), pan[3], fader[3]);

  mix_aux_mono(in[0], &(out[2]), &(out[3]), auxL[0], auxR[0]);
  mix_aux_mono(in[1], &(out[2]), &(out[3]), auxL[1], auxR[1]);
  mix_aux_mono(in[2], &(out[2]), &(out[3]), auxL[2], auxR[2]);
  mix_aux_mono(in[3], &(out[2]), &(out[3]), auxL[3], auxR[3]);

  //define delay input & output

  //mix adcs to delay inputs
  delayInput = mult_fr1x32x32(in[3],effect[3]) ;
  delayInput = add_fr1x32(delayInput, mult_fr1x32x32(in[2],effect[2]));
  delayInput = add_fr1x32(delayInput, mult_fr1x32x32(in[1],effect[1]));
  delayInput = add_fr1x32(delayInput, mult_fr1x32x32(in[0],effect[0]));

  delayInput = add_fr1x32(delayInput, mult_fr1x32x32(delayOutput,feedback));

  delayOutput = delay_next( &(lines[0]), delayInput);



  mix_panned_mono(delayOutput, &(out[1]), &(out[0]),PAN_DEFAULT ,FADER_DEFAULT );
}

void mix_aux_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue left_value, ParamValue right_value) {

    *out_right = add_fr1x32(*out_right,mult_fr1x32x32(in_mono, right_value));

    *out_left = add_fr1x32(*out_left,mult_fr1x32x32(in_mono, left_value));

}


void mix_panned_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) {
    fract32 pan_factor, post_fader;

    //pan_factor = (fract32)pan;
    //*out_left += mult_fr1x32x32(in_mono,pan_factor); //debug

    pan_factor = (fract32) ( pan );
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_left = add_fr1x32(*out_left, mult_fr1x32x32(in_mono, post_fader));

    pan_factor = (fract32) ( PAN_MAX - pan );
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_right = add_fr1x32(*out_right, mult_fr1x32x32(in_mono, post_fader));

}


// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE;
  switch(idx) {
    // dac values
  case eParam_dac0 :
   filter_1p_lo_in(&(dacSlew[0]), shr_fr1x32(v, PARAM_DAC_RADIX - 1));
   //     dac_update(0, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_dac1 :
    //dac_update(1, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_dac2 :
    filter_1p_lo_in(&(dacSlew[2]), shr_fr1x32(v, PARAM_DAC_RADIX - 1));
    //dac_update(2, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_dac3 :
     filter_1p_lo_in(&(dacSlew[3]), shr_fr1x32(v, PARAM_DAC_RADIX - 1));
     //   dac_update(3, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_slew0 :
   filter_1p_lo_set_slew(&(dacSlew[0]), v);
    break;
  case eParam_slew1 :
    filter_1p_lo_set_slew(&(dacSlew[1]), v);
    break;
  case eParam_slew2 :
    filter_1p_lo_set_slew(&(dacSlew[2]), v);
    break;
  case eParam_slew3 :
    filter_1p_lo_set_slew(&(dacSlew[3]), v);
    break;
  case eParam_auxL0 :
    auxLTarget[0] = v;
    break;
  case eParam_auxR0 :
    auxRTarget[0] = v;
    break;
  case eParam_pan0 :
    panTarget[0] = v;
    break;
  case eParam_fader0 :
    faderTarget[0] = v;
    break;
  case eParam_effect0 :
    effectTarget[0] = v;
    break;
  case eParam_auxL1 :
    auxLTarget[1] = v;
    break;
  case eParam_auxR1 :
    auxRTarget[1] = v;
    break;
  case eParam_pan1 :
    panTarget[1] = v;
    break;
  case eParam_fader1 :
    faderTarget[1] = v;
    break;
  case eParam_effect1 :
    effectTarget[1] = v;
    break;
  case eParam_auxL2 :
    auxLTarget[2] = v;
    break;
  case eParam_auxR2 :
    auxRTarget[2] = v;
    break;
  case eParam_pan2 :
    panTarget[2] = v;
    break;
  case eParam_fader2 :
    faderTarget[2] = v;
    break;
  case eParam_effect2 :
    effectTarget[2] = v;
    break;
  case eParam_auxL3 :
    auxLTarget[3] = v;
    break;
  case eParam_auxR3 :
    auxRTarget[3] = v;
    break;
  case eParam_pan3 :
    panTarget[3] = v;
    break;
  case eParam_fader3 :
    faderTarget[3] = v;
    break;
  case eParam_effect3 :
    effectTarget[3] = v;
    break;
  case eParam_feedback0 :
    feedbackTarget = v;
    break;
  case eParam_delay0 :
    //delayTimeTarget = v;
    //filter_1p_lo_in(&delayTimeSlew, v);
    delay_set_rate(&(lines[0]), v/64);
    break;
  case eParam_delay0Slew :
    filter_1p_lo_set_slew(&delayTimeSlew, v);
  default:
    break;
  }
}
