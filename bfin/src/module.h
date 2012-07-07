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

//-------- global variables
// parameter descriptor table
extern ParamDesc paramDesc[];
// parameter data table
extern ParamData paramData[];
// count of parameters
extern const u32 kNumParams;

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
