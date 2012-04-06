
/* param.h
 * aleph-blackfin modules
 * 
 * public interface for parameter access
 */

#ifndef _ALEPH_BFIN_PARAM_H_
#define _ALEPH_BFIN_PARAM_H_

#include <fract.h>
#include <fract_typedef.h>
#include "util.h"
#include "types.h"
#include "param.h"

//-------- general defines
// max characters in param name
#define PARAM_NAME_LEN 32
// max charactes in param unit
#define PARAM_UNIT_LEN 8
// TODO: define units?

//-------- data types

typedef enum {
  eParamFloat,
  eParamFract,
  eParamInt,
  eNumParamTypes
} eParamType;

// parameter descriptor
// this is stuff like parameter names and other attributes
// that can be stored in SDRAM for infrequent access
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char name[PARAM_NAME_LEN];
  // parameter unit name
  char unit[PARAM_UNIT_LEN];
  // type: fract, int, or float
  eParamType type;
  // other stuff: range? curve?
} ParamDesc;

// current realtime param data, should be stored in L1
typedef struct __attribute__((__packed__)) ParamDataStruct {
  // a 4-byte union. the correct interpretation should be chosen for a given parameter's datatype.
  union {
    u32      asInt32; 
    fract32  asFract;
    f32      asFloat; 
  } value;
  // other stuff? (interpolation state?)
} ParamData;



/*
//-------- function prototypes
// get the name of a parameter.
char* getParamName(u32 idx);
//  get the value of a parameter of specific type
f32     getParamValueFloat(u32 idx);
fract32 getParamValueFract(u32 idx);
u32     getParamValueUnsigned(u32 idx);
// set the value of a parameter.
void setParamValue(u32 idx, u32 val);
*/

#endif // header guard
