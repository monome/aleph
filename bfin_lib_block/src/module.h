/*! module.h
 
  static base class for audio modules.

 */


#ifndef _ALEPH_MODULE_H_
#define _ALEPH_MODULE_H_

#include "audio.h"
#include "module_common.h"
#include "param_common.h"
#include "types.h"

//! global module data type
typedef struct _moduleData {
  char name[MODULE_NAME_LEN];
  ParamData* paramData; 
  u16 numParams;
} ModuleData;

//! global pointer to module data
extern ModuleData* gModuleData;

//! initialize the module
extern void module_init(void);

//! process a block of frames
extern void module_process_block(buffer_t *inChannels, buffer_t *outChannels);

//! set parameter  
extern void module_set_param(u32 idx, ParamValue val);

#endif
