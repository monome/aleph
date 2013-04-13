/* module.h
 * null
 * aleph
 *
 * module header for "null" audio engine
 */


#ifndef _NULLP_MODULE_INTERFACE_H_
#define _NULLP_MODULE_INTERFACE_H_

#include "module_common.h"
#include "param_common.h"
#include "types.h"

//-----------------------
//---- module descriptor
typedef struct _moduleDesc {
  char name[MODULE_NAME_LEN];
  ParamDesc* paramDesc;
  ParamData* paramData;
  u16 numParams;
} moduleData_t;

//typedef union { u32 u; fix16 fix; fract32 fr; float f; } pval;

extern moduleData_t* moduleData;

// debug output flag
extern u8 dbgFlag;

//-------- function prototypes 
// init
extern void module_init();
// de-init
extern void module_deinit(void);
// callback
extern void module_process_frame(const f32* in, f32* out);
// set parameter
extern void module_set_param(u32 idx, pval val);
// get number of parameters
extern u32 module_get_num_params(void);


#endif // header guard
