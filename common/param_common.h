
/* param_common.h
 * aleph-common
 *
 * common parameter data types
 */

#ifndef _ALEPH_COMMON_PARAM_H_
#define _ALEPH_COMMON_PARAM_H_

#include "fix.h"
//#include "types.h"

// max characters in param name
#define PARAM_LABEL_LEN 16

// data type
typedef s32 ParamValue;

///--- parameter use-types
/// these differentiate  the actual, numerical use case for the parameter.
// add as needed, sync with param scaler definitions
typedef enum {
  // 0 boolean
  eParamTypeBool,
  // 1 fixed-point linear (radix is stored separately)
  eParamTypeFix,
  // 2 amplitude (0-1)
  eParamTypeAmp,
  // 3 32b integrator time/coefficient
  eParamTypeIntegrator,
  // 4 note number -> hz
  eParamTypeNote,
  // 5 frequency coefficient for SVF
  eParamTypeSvfFreq,
  // 6 generic signed fract32
  // controller values will be quantized to 16b 
  eParamTypeFract,
  // 7 generic 16b signed integer
  eParamTypeShort,   
  // 8 16b integrator time/coefficient
  eParamTypeIntegratorShort,  

  eParamNumTypes
} ParamType;

// a union type for byteswapping
typedef union __attribute__((__packed__)) {
  s32 asInt;
  u32 asUint;
  u8 asByte[4];
} ParamValueSwap;

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
