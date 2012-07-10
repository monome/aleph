
/* param.h
 * aleph-common
 * 
 * common parameter data types
 */

#ifndef _ALEPH_COMMON_PARAM_H_
#define _ALEPH_COMMON_PARAM_H_

#include "types.h"

// max characters in param name
#define PARAM_NAME_LEN 18
// max charactes in param unit
#define PARAM_UNIT_LEN 8
// TODO: define units?

//-------- data typessp
typedef enum {
  eParamFloat,
  eParamFract,
  eParamInt, // unsigned ? 
  eNumParamTypes
} eParamType;

// 4-byte union of value representations
typedef union {
  f32 asFloat;
  fract32 asFract;
  u8 asByte[4];
  u16 asShort[2];
} ParamValue;

// parameter descriptor
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char name[PARAM_NAME_LEN];
  // parameter unit name
  char unit[PARAM_UNIT_LEN];
  // type: fract, int, or float
  eParamType type;
  // other stuff: range? curve?
} ParamDesc;

// parameter data
typedef struct __attribute__((__packed__)) ParamDataStruct {
  volatile ParamValue value;
  volatile u8 changed;
} ParamData;

#endif // header guard
