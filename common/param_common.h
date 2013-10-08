
/* param.h
 * aleph-common
 * 
 * common parameter data types
 */

#ifndef _ALEPH_COMMON_PARAM_H_
#define _ALEPH_COMMON_PARAM_H_

#include "fix.h"
#include "types.h"

//======@@@@@@@++++++########!!!!!!!
//!!!!!!====== BEWARE !!!!!!********
/// there are all kinds of dirty hacks 
/// dependent on the layout of data types in this header:
///// in the bytecount #defines in protocol.h
///// in param.c:get_param_value
/////____change the layouts only with extraordinary caution!!! _ez

// max characters in param name
#define PARAM_LABEL_LEN 16
// max charactes in param unit
#define PARAM_UNIT_LEN 8

//---- param data types
// #define PARAM_TYPE_FIX 0
// #define PARAM_TYPE_FRACT 0
// #define PARAM_TYPE_UINT  1


///--- parameter use-types
/// these diferentiate  the actual, numerical use case for the parameter.
typedef enum {
  // raw amplitude
  eParamTypeAmp,
  // count of samples
  eParamTypeSamples,
  // raw coefficient for 1pole lowpass
  eParamType1plpCoeff,
  // raw coefficient for SVF
  eParamTypeSvfCoeff,
} paramUseType;

// 4-byte union of value representations
typedef union __attribute__((__packed__)) {
  f32 asFloat;
  fract32 asFract;
  s32 asInt;
  u32 asUint;
  u8 asByte[4];
  u16 asShort[2];
} ParamValue;

// a simpler form for internal use
typedef union { u32 u; s32 s; fix16 fix; fract32 fr; } pval;

// parameter descriptor
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char label[PARAM_LABEL_LEN];
  // parameter unit name
  char unit[PARAM_UNIT_LEN];
  // type
  u8 type;
  // range
  fix16 min;
  fix16 max;
  // other stuff? id for mapping? (hz, pole, db, rate, etc?)
} ParamDesc;

// parameter data
typedef struct __attribute__((__packed__)) ParamDataStruct {
  volatile ParamValue value;
  volatile u8 changed;
} ParamData;

#endif // header guard
