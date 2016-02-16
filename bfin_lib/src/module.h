/* module.h
   aleph-audio
   
   class header for audio-processing module.
 */


#ifndef _ALEPH_AUDIO_MODULE_H_
#define _ALEPH_AUDIO_MODULE_H_

#include "module_common.h"
#include "param_common.h"
#include "types.h"

#ifndef SAMPLERATE
#define SAMPLERATE    48000
#endif

#if ARCH_BFIN

#define SDRAM_ADDRESS 0x00000000
#define SDRAM_SIZE    0x07ffffff


#else
void *SDRAM_ADDRESS;
#define IN_PORTS 4
#define OUT_PORTS 4
#define SDRAM_SIZE 0x07ffffff

fract32 in[IN_PORTS];
fract32 out[OUT_PORTS];

void module_init(void);
void module_process_frame(void);


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

// debug output flag
extern u8 dbgFlag;

//-------- function prototypes 
// init
extern void module_init(void);
// de-init
extern void module_deinit(void);
// callback
extern void module_process_frame(void);

// set parameter  
extern void module_set_param(u32 idx, ParamValue val);

// get number of parameters
extern u32 module_get_num_params(void);


#endif // header guard
