/*! 
  module.h
  bfin_lib
  aleph 

  static base class for audio modules.

 */


#ifndef _BFIN_MODULE_H_
#define _BFIN_MODULE_H_

// common
#include "module_common.h"
#include "param_common.h"
#include "types.h"
// bfin_lib_block
#include "audio.h"


#define SDRAM_ADDRESS 0x00000000
#define SDRAM_SIZE    0x07ffffff
#ifndef SAMPLERATE
#define SAMPLERATE    48000
#endif


//-----------------------
//---- module descriptor
typedef struct _moduleData {
  char name[MODULE_NAME_LEN];
  ParamData* paramData; 
  u16 numParams;
} ModuleData;

//---------
// global pointer to module descriptor
extern ModuleData* gModuleData;

//! initialize the module
extern void module_init(void);

//! process a block of samples
extern void module_process_block(void);

//! set parameter  
extern void module_set_param(u32 idx, ParamValue val);

//! get number of parameters
extern u32 module_get_num_params(void);

#endif
