/* module.h
 * bfin-alpeh core audio system
 * 
 * declares variables and functions to be defined by custom modules
 */

#ifndef _BF533_CORE_MODULE_H_
#define _BF533_CORE_MODULE_H_

#include "types.h"
#include "param.h"

#define SDRAM_ADDRESS 0x00000000
#define SDRAM_SIZE    0x07ffffff

#define SAMPLERATE 48000

//-----------------------
//---- module data
typedef struct _moduleData {
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
