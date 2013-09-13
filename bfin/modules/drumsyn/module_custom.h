/* module_custom.h
   
   some consants should be both customized for each module,
   and preprocessed for efficiency.
   here is where such things live.

   most likely, this is just a count of parameters.
   
   a copy of this header should exist in your custom modiule directory 
   (e.g. aleph/bfin/modules/mymodule/
 */

#ifndef _ALEPH_MODULE_CUSTOM_H_
#define _ALEPH_MODULE_CUSTOM_H_

#include "params.h"

enum {
  eParam_ampAtk,
  eParam_ampSus,
  eParam_ampRel,  

  eParam_hzStart,
  eParam_hzEnd,
  eParam_hzAtk,
  eParam_hzSus,
  eParam_hzRel,  

  eParam_resStart,
  eParam_resEnd,
  eParam_resAtk,
  eParam_resSus,
  eParam_resRel,  

  eParam_svfLow,
  eParam_svfHigh,
  eParam_svfBand,

};


#define NUM_PARAMS eParamNumParams

#endif // h guard
