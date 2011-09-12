/* echo_data.c
 * aleph-blackfin echo module
 *
 * defines external data for echo module.
 * this includes param descriptors and any runtime variables that are too big for internal memory.
 * the resulting output should be linked to SDRAM section in memory map
 */

#include "param.h"
#include "echo_data.h"

ParamDesc paramDesc[MODULE_NUM_PARAMS] = {
  {"time", eParamFloat},  
  {"mix", eParamFract},
  {"feedback", eParamFract}
};

// EXTERNAL ARRAYS:
// array must be assigned here to get linked to SDRAM...
// an empty assignment will put it in .sdram.bss,
// a real assignment will go in .sdram.data 
fract32 echoBuf[ECHO_NUMFRAMES * 4] = {};
