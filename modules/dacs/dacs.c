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

// data structure of external memory
typedef struct _linesData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} dacsData;

//-------------------------
//----- extern vars (initialized here)
moduleData* gModuleData; 
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

  gModuleData->paramDesc = (ParamDesc*)pDacsData->mParamDesc;
  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(dacSlew[0]), 0 );
  filter_1p_lo_init( &(dacSlew[1]), 0 );
  filter_1p_lo_init( &(dacSlew[2]), 0 );
  filter_1p_lo_init( &(dacSlew[3]), 0 );

  fill_param_desc();
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

void module_process_frame(void) { 
  //  u8 i;
  
  //  for(i=0; i<4; ++i) {
    //    if(dacSlew[i].sync) { continue; }
    //    dacVal[i] = filter_1p_lo_next(&(dacSlew[i]));
    //    dac_update(i, dacVal[i] & DAC_VALUE_MASK);

    /* if(dacDirty[dacChan]) { */
    /*   dac_update(dacChan, dacVal[dacChan]); */
    /*   dacDirty[dacChan] = 0; */
    /* } */

  
  if(dacSlew[dacChan].sync) { ;; } else {
    dacVal[dacChan] = filter_1p_lo_next(&(dacSlew[dacChan]));
    dac_update(dacChan, shr_fr1x32(dacVal[dacChan], 15) & DAC_VALUE_MASK);
  }
 
  if(++dacChan == 4) {
    dacChan = 0;
  }
  
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];
  out[3] = in[3];
  
    //  } 
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
    filter_1p_lo_in(&(dacSlew[1]), shr_fr1x32(v, PARAM_DAC_RADIX - 1));
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
  default:
    break;
  }
}
