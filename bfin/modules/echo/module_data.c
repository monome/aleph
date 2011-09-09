/* module-data.c
 * aleph-blackfin echo module
 *
 * defines external data. 
 *
 * this includes param descriptors and any runtime variables that are too big for internal memory.
 * the resulting output should be linked to SDRAM data section.
 *
 * for now, this sourcefile MUST have the same name in each module as it is hardcoded into the link script.
*/

#include "../../param.h"
#include "echo_data.h"

ParamDesc paramDesc[MODULE_NUM_PARAMS] = {
  {"time", eParamFloat},  
  {"mix", eParamFract},
  {"feedback", eParamFract}
};

fract32 echoBuf[ECHO_NUMFRAMES * 4];
