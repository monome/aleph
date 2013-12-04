
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

// data type
typedef s32 ParamValue;

///--- parameter use-types
/// these differentiate  the actual, numerical use case for the parameter.
// add as needed, sync with param scaler definitions
typedef enum {
  /// fixed-point linear (radix is stored separately)
  eParamTypeFix,
  // amplitude (0-1)
  eParamTypeAmp,
  // integrator time/coefficient
  eParamTypeIntegrator,
  // time in samples
  //  eParamTypeSamples,
  // frequency coefficient for 1pole filter
  //  eParamType1pFreq,
  // oscillator, base frequency
  //  eParamTypeOscFreq,
  // oscillator tuning ratio
  //  eParamTypeOscTune,
  // frequency coefficient for SVF
  //  eParamTypeSvfFreq,
  // RQ coefficient for SVF
  //  eParamTypeSvfRq,
  //...?
  eParamNumTypes
} ParamType;

// a union type for byteswapping
typedef union __attribute__((__packed__)) {
  s32 asInt;
  u32 asUint;
  u8 asByte[4];
} ParamValueCommon;

// parameter descriptor
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char label[PARAM_LABEL_LEN];
  // type
  ParamType type;
  // range (may be independent of type) 
  s32 min;
  s32 max;
  // radix = integer bits (including sign bit) :
  // 1 = signed fract (1.32)
  // 16 = 16.16
  // 32 = signed int
  u8 radix;
} ParamDesc;

// parameter data
typedef struct __attribute__((__packed__)) ParamDataStruct {
  ParamValue value;
  // not sure we need to use this but keep it around and updated
  u8 changed;
} ParamData;

#endif // header guard
