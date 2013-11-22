
/* param_common.h
 * aleph-common
 * 
 * common parameter data types
 */

#ifndef _ALEPH_COMMON_PARAM_H_
#define _ALEPH_COMMON_PARAM_H_

#include "fix.h"
#include "types.h"

// max characters in param name
#define PARAM_LABEL_LEN 16

///--- parameter use-types
/// these diferentiate  the actual, numerical use case for the parameter.
// add as needed, sync with param scaler definitions
typedef enum {
  // amplitude (0-1)
  eParamTypeAmp,
  // time in samples
  eParamTypeSamples,
  // frequency coefficient for 1pole filter
  eParamType1pFreq,
  // frequency coefficient for SVF
  eParamTypeSvfFreq,
  // RQ coefficient for SVF
  eParamTypeSvfRq,
  // oscillator, base frequency
  eParamTypeOscFreq,
  // oscillator tuning ratio
  eParamTypeOscTune,
  //...?
  eParamNumTypes
} ParamType;


// a union type for byteswapping
typedef union __attribute__((__packed__)) {
  s32 asInt;
  u32 asUint;
  u8 asByte[4];
} ParamValueCommon;

// data type
typedef s32 ParamValue;

// parameter descriptor
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char label[PARAM_LABEL_LEN];
  // type
  ParamType type;
  fix16 min;
  fix16 max;
} ParamDesc;

// parameter data
typedef struct __attribute__((__packed__)) ParamDataStruct {
  ParamValue value;
  // not sure we need to use this but keep it around and updated
  u8 changed;
} ParamData;

#endif // header guard
