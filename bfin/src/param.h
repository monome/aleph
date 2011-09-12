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

//-------- general defines
// max characters in param name
#define PARAM_NAME_LEN 32

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
// current param data and interpolation should be stored in L1 memory
typedef struct __attribute__((__packed__)) ParamDescStruct {
  // parameter name
  char name[PARAM_NAME_LEN];
  // type: fract, int, or float
  eParamType type;
} ParamDesc;

typedef struct __attribute__((__packed__)) ParamDataStruct {
  // a 4-byte union. the correct interpretation should be chosen for a given parameter's datatype.
  union { u32 ui; s32 si; f32 f; } value;
} ParamData;

//-------- function prototypes
// get the name of a parameter.
char* getParamName(u32 idx);
//  get the value of a parameter of specific type
f32     getParamValueFloat(u32 idx);
fract32 getParamValueFract(u32 idx);
u32     getParamValueUnsigned(u32 idx);
// set the value of a parameter.
void setParamValue(u32 idx, u32 val);


#endif // header guard
