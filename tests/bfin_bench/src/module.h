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
extern void module_init(void);
// frame processing function
extern void module_process_frame(void);
// button handler
extern void module_handle_button(void);
// led update
extern u8 module_update_leds(void);

#endif // header guard
