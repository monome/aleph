/* module.h
   aleph-audio
   
   class header for audio-processing module.
 */


#ifndef _ALEPH_AUDIO_MODULE_H_
#define _ALEPH_AUDIO_MODULE_H_

#include "module_common.h"
#include "param_common.h"
#include "types.h"

//#if ARCH_BFIN

#define SDRAM_ADDRESS 0x00000000
#define SDRAM_SIZE    0x07ffffff
#ifndef SAMPLERATE
  #define SAMPLERATE    48000
#endif

//#endif

//-----------------------
//---- module descriptor
typedef struct _moduleData {
  char name[MODULE_NAME_LEN];
  ParamDesc* paramDesc;
  ParamData* paramData;
  u16 numParams;
} moduleData;

//---------
// global pointer to module descriptor
extern moduleData* gModuleData;

// debug output flag
extern u8 dbgFlag;

//-------- function prototypes 
// init
extern void module_init(void);
// de-init
extern void module_deinit(void);
// callback

#if ARCH_BFIN
extern void module_process_frame(void);
#else
extern void module_process_frame(const f32* in, f32* out);
#endif

// set parameter  
extern void module_set_param(u32 idx, pval val);
// get number of parameters
extern u32 module_get_num_params(void);


#endif // header guard




 
