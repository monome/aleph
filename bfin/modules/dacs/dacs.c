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


void module_process_frame(void) { 
  u8 i;
  for(i=0; i<4; ++i) {
    if(dacSlew[i].sync) { continue; }
    dacVal[i] = filter_1p_lo_next(&(dacSlew[i]));
    dac_update(i, dacVal[i] & 0xffff);
  }
}

// parameter set function
void module_set_param(u32 idx, pval v) {
  LED4_TOGGLE;
  switch(idx) {
    // dac values
  case eParam_dac0 :
        filter_1p_lo_in(&(dacSlew[0]), v.fr);
    //    dac_update(0, v.fr & 0xffff);
    break;
  case eParam_dac1 :
    filter_1p_lo_in(&(dacSlew[1]), v.fr);
    //    dac_update(1, v.fr & 0xffff);
    break;
  case eParam_dac2 :
    filter_1p_lo_in(&(dacSlew[2]), v.fr);
    //    dac_update(2, v.fr & 0xffff);
    break;
  case eParam_dac3 :
    filter_1p_lo_in(&(dacSlew[3]), v.fr);
    //    dac_update(3, v.fr & 0xffff);
    break;
  case eParam_slew0 :
    filter_1p_lo_set_slew(&(dacSlew[0]), v.fr);
    break;
  case eParam_slew1 :
    filter_1p_lo_set_slew(&(dacSlew[1]), v.fr);
    break;
  case eParam_slew2 :
    filter_1p_lo_set_slew(&(dacSlew[2]), v.fr);
    break;
  case eParam_slew3 :
    filter_1p_lo_set_slew(&(dacSlew[3]), v.fr);
    break;
  default:
    break;
  }
}
