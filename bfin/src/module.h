/* module.h
 * bfin-alpeh core audio system
 * 
 * declares variables and functions to be defined by custom modules
 */

#ifndef _BF533_CORE_MODULE_H_
#define _BF533_CORE_MODULE_H_

#include "module_common.h"
#include "param_common.h"
#include "types.h"

#define SDRAM_ADDRESS 0x00000000
#define SDRAM_SIZE    0x07ffffff

#define SAMPLERATE    48000

//inline void* get_sdram_address(void) { return (void*)SDRAM_ADDRESS; }
//inline const int get_sdram_size(void) { return SDRAM_SIZE; }

//-----------------------
//---- module data
typedef struct _moduleData {
  char name[MODULE_NAME_LEN];
  ParamDesc* paramDesc;
  ParamData* paramData;
  u16 numParams;
} moduleData_t;

extern moduleData_t* moduleData;

//-------- function prototypes 
// intialize
void init_module(void);
// frame processing function
void process_frame(void);
// set parameter value
// get parameter value
// get parameter type
// get parameter 


#endif // header guard
