/* echo_data.c
 * aleph-blackfin echo module
 *
 * defines external data for echo module.
 * this includes param descriptors and any runtime variables that are too big for internal memory.
 * the resulting output should be linked to SDRAM section in memory map.
 */

#include "module.h"
#include "param_common.h"
#include "echo_data.h"

// define twice for preprocesor and core code modules
const u32 kNumParams = MODULE_NUM_PARAMS;

ParamDesc desc[MODULE_NUM_PARAMS] = {
  {"mix0", "amp", eParamFract},
  {"mix1", "amp",  eParamFract},
  {"mix2", "amp",  eParamFract},
  {"mix3", "amp",  eParamFract},
  {"time0", "sec", eParamFloat},
  {"time1", "sec", eParamFloat},  
  {"time2", "sec", eParamFloat},  
  {"time3", "sec", eParamFloat}, 
  {"feedback00", "amp", eParamFract},
  {"feedback01", "amp", eParamFract},
  {"feedback02", "amp", eParamFract},
  {"feedback03", "amp", eParamFract},
  {"feedback10", "amp", eParamFract},
  {"feedback11", "amp", eParamFract},
  {"feedback12", "amp", eParamFract},
  {"feedback13", "amp", eParamFract},
  {"feedback20", "amp", eParamFract},
  {"feedback21", "amp", eParamFract},
  {"feedback22", "amp", eParamFract},
  {"feedback23", "amp", eParamFract},
  {"feedback30", "amp", eParamFract},
  {"feedback31", "amp", eParamFract},
  {"feedback32", "amp", eParamFract},
  {"feedback33", "amp", eParamFract},
};

// EXTERNAL ARRAYS:
// array must be assigned here to get linked to SDRAM...
// an empty assignment will put it in .sdram.bss,
// a real assignment will go in .sdram.data 
//fract32 echoBuf[ECHO_NUMSAMPS] = {};

/*
actually, having toruble making a working .ldr
using the custom-memory-map method of linking SDRAM.
falling back on this very hackish style solution.
*/

fract32 * echoBuf = SDRAM_ADDRESS;
