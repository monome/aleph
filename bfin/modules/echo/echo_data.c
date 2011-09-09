/* echo_data.c
 * aleph-blackfin echo module
 *
 * defines external data for echo module.
 * this includes param descriptors and any runtime variables that are too big for internal memory.
 * the resulting output should be linked to SDRAM section in memory map
 */

#include "../../param.h"
#include "echo_data.h"

ParamDesc paramDesc[MODULE_NUM_PARAMS] = {
  {"time", eParamFloat},  
  {"mix", eParamFract},
  {"feedback", eParamFract}
};

fract32 echoBuf[ECHO_NUMFRAMES * 4];
