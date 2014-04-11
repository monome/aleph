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

/// custom
#include "params.h"


// total SDRAM is 64M
// each line 60ish seconds for now
#define LINES_BUF_FRAMES 0x2bf200
// try...
//#define LINES_BUF_FRAMES 0x600000
//#define LINES_BUF_FRAMES 0x1000000
//#define LINES_BUF_FRAMES 0xbb8000 // 256 seconds @ 48k
#define NLINES 2

// data structure of external memory
typedef struct _dacsData {
  ModuleData super;
  //ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
//  volatile fract32 audioBuffer[NLINES][LINES_BUF_FRAMES];
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

void module_init(void) {
  // init module/param descriptor
  pDacsData = (dacsData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "aleph-dacs");

  //gModuleData->paramDesc = (ParamDesc*)pDacsData->mParamDesc;
  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(dacSlew[0]), 0 );
  filter_1p_lo_init( &(dacSlew[1]), 0 );
  filter_1p_lo_init( &(dacSlew[2]), 0 );
  filter_1p_lo_init( &(dacSlew[3]), 0 );

  //fill_param_desc();
}

// de-init
void module_deinit(void) {
  ;;
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
/// 

#define PAN_MAX 2147483647

ParamValue auxL[4];
ParamValue auxR[4];
ParamValue pan[4];
ParamValue fader[4];
//ParamValue eq_hi[4];
//ParamValue eq_mid[4];
//ParamValue eq_lo[4];

void mix_aux_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) ;

void mix_panned_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) ;

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

  //Transfer audio from inputs to outputs
  //out[0] = in[0];
  //out[1] = in[1];
  //out[2] = in[2];
  //out[3] = in[3];

  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;


  mix_panned_mono(in[0], &(out[1]), &(out[0]), pan[0], fader[0]);
  mix_panned_mono(in[1], &(out[1]), &(out[0]), pan[1], fader[1]);
  mix_panned_mono(in[2], &(out[1]), &(out[0]), pan[2], fader[2]);
  mix_panned_mono(in[3], &(out[1]), &(out[0]), pan[3], fader[3]);

  mix_aux_mono(in[0], &(out[2]), &(out[3]), auxL[0], auxR[0]);
  mix_aux_mono(in[1], &(out[2]), &(out[3]), auxL[1], auxR[1]);
  mix_aux_mono(in[2], &(out[2]), &(out[3]), auxL[2], auxR[2]);
  mix_aux_mono(in[3], &(out[2]), &(out[3]), auxL[3], auxR[3]);
}

void mix_aux_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue left_value, ParamValue right_value) {

    *out_right += mult_fr1x32x32(in_mono, right_value);

    *out_left += mult_fr1x32x32(in_mono, left_value);

}


void mix_panned_mono(fract32 in_mono, fract32* out_left, fract32* out_right, ParamValue pan, ParamValue fader) {
    fract32 pan_factor, post_fader;

    //pan_factor = (fract32)pan;
    //*out_left += mult_fr1x32x32(in_mono,pan_factor); //debug

    pan_factor = (fract32) ( pan );
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_left += mult_fr1x32x32(in_mono, post_fader);

    pan_factor = (fract32) ( PAN_MAX - pan );
    post_fader = mult_fr1x32x32(pan_factor, fader);
    *out_right += mult_fr1x32x32(in_mono, post_fader);

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
    auxL[0] = v;
    break;
  case eParam_auxR0 :
    auxR[0] = v;
    break;
  case eParam_pan0 :
    pan[0] = v;
    break;
  case eParam_fader0 :
    fader[0] = v;
    break;
  case eParam_auxL1 :
    auxL[1] = v;
    break;
  case eParam_auxR1 :
    auxR[1] = v;
    break;
  case eParam_pan1 :
    pan[1] = v;
    break;
  case eParam_fader1 :
    fader[1] = v;
    break;
  case eParam_auxL2 :
    auxL[2] = v;
    break;
  case eParam_auxR2 :
    auxR[2] = v;
    break;
  case eParam_pan2 :
    pan[2] = v;
    break;
  case eParam_fader2 :
    fader[2] = v;
    break;
  case eParam_auxL3 :
    auxL[3] = v;
    break;
  case eParam_auxR3 :
    auxR[3] = v;
    break;
  case eParam_pan3 :
    pan[3] = v;
    break;
  case eParam_fader3 :
    fader[3] = v;
    break;
  default:
    break;
  }
}
