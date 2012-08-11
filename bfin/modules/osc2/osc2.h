
/* echo_data.h
 * aleph-blackfin echo module
 *
 * declares external data
 */

#ifndef _ALEPH_BFIN_OSC2_H_
#define _ALEPH_BFIN_OSC2_H_

#include "module.h"
#include "param.h"
#include "types.h"

//#define OSC_SAMPLERATE 48000

#define MODULE_NUM_PARAMS 2
#define WAVETABLE_NUM 1024
#define WAVETABLE_NUM_1 1023

// parameter indices
enum {
  //  eParamAmp   ,
  eParamFreq1 = 0,
  eParamFreq2 = 1,
};

// module data structure
// inhereits from moduleData_t
// located in external memory
typedef struct _oscData {
  moduleData_t super;
  //// private:
// parameter descriptors
  ParamDesc paramDesc_p[MODULE_NUM_PARAMS];
  // parameter data
  ParamData paramData_p[MODULE_NUM_PARAMS];
  // sine wavetable
  f32 sineTable[WAVETABLE_NUM];
} oscData_t;

#endif // header guard
