
/* param.h
 * aleph-common
 * 
 * common parameter data types
 */

#ifndef _ALEPH_COMMON_PARAM_H_
#define _ALEPH_COMMON_PARAM_H_

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
// TODO: define units?

//-------- param types
#define PARAM_TYPE_FLOAT 0
#define PARAM_TYPE_FRACT 1
#define PARAM_TYPE_UINT  2

// 4-byte union of value representations
typedef union __attribute__((__packed__)) {
  f32 asFloat;
  fract32 asFract;
  u32 asUint;
  u8 asByte[4];
  u16 asShort[2];
} ParamValue;

// parameter descriptor
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char label[PARAM_LABEL_LEN];
  // parameter unit name
  char unit[PARAM_UNIT_LEN];
  // type: fract, int, or float
  u8 type;
  // range
  f32 min;
  f32 max;
  // other stuff? curve? scaling table?
} ParamDesc;

// parameter data
typedef struct __attribute__((__packed__)) ParamDataStruct {
  volatile ParamValue value;
  volatile u8 changed;
} ParamData;

#endif // header guard
