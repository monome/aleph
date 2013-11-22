
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
typedef enum {
  // raw amplitude
  eParamTypeAmp,
  // time in samples
  eParamTypeSamples,
  // raw coefficient for 1pole lowpass
  eParamType1plpCoeff,
  // raw coefficient for SVF
  eParamTypeSvfCoeff,
  // oscillator, normalized frequency
  eParamTypeOscFreq,
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
} ParamDesc;

// parameter data
typedef struct __attribute__((__packed__)) ParamDataStruct {
  ParamValue value;
  // not sure we need to use this but keep it around and updated
  u8 changed;
} ParamData;

#endif // header guard
