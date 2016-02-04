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

#define NUM_PARAMS eParamNumParams

#endif
