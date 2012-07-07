
/* echo_data.h
 * aleph-blackfin echo module
 *
 * declares external data
 */

#ifndef _ALEPH_BFIN_ECHO_DATA_H_
#define _ALEPH_BFIN_ECHO_DATA_H_

#include "types.h"

//#define OSC_SAMPLERATE 48000

#define MODULE_NUM_PARAMS 2
#define WAVETABLE_NUM 1024

// parameter indices
enum {
  eParamAmp  ,
  eParamFreq
};

// define a module data structure
// this should be everything to be located in external memory
typedef struct _oscData {
// parameter descriptors
  ParamDesc paramDesc[MODULE_NUM_PARAMS];
  // parameter data
  ParamData paramData[MODULE_NUM_PARAMS];
  // sine wavetable
  const fract32 sineTable[WAVETABLE_NUM];
} oscData_t;

// manuale locate the data structure at start of SDRAM
extern oscData_t * data;

#endif // header guard
