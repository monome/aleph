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

#include "bfin_core.h"
#include "dac.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio
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
  ;;
}

// parameter set function
void module_set_param(u32 idx, pval v) {
  switch(idx) {
    // delay line params
  case eParam_dac0 :
    dac_update(0, v.u & 0xffff);
    break;
  case eParam_dac1 :
    dac_update(1, v.u & 0xffff);
    break;
  case eParam_dac2 :
    dac_update(2, v.u & 0xffff);
    break;
  case eParam_dac3 :
    dac_update(3, v.u & 0xffff);
    break;

  default:
    break;
  }
}
