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

extern moduleData_t* moduleData;

//-------- function prototypes 
// init
extern void module_init(const u32 samplerate);
// de-init
extern void module_deinit(void);
// callback
extern void module_process_frame(const f32* in, f32* out);
// set parameter
extern void module_set_param(u32 idx, f32 val);
// get parameter description given indexx
extern void module_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc);
// get number of parameters


#endif // header guard
