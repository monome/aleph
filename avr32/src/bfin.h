/* bfin.h
   aleph-avr32

   routines to communicate with the bf533 DSP
*/

#ifndef _BFIN_H_
#define _BFIN_H_

#include "compiler.h"
#include "fix.h"
#include "param_common.h"
#include "types.h"

//#define USE_LDR_RAM_BUFFER 1

// blackfin HWAIT status
extern volatile U8 hwait;

// load a blackfin executable
#ifdef USE_LDR_RAM_BUFFER
void bfin_load(U32 size, volatile u8 * data);
#else 
void bfin_load(U32 size, void* fp);
#endif

// set a parameter
//void bfin_set_param(U8 idx, F32 val);
void bfin_set_param(U8 idx, fix16_t val);

// get number of parameters 
void bfin_get_num_params(volatile u32* num);
// get a parameter name
// void bfin_get_param_name(u16 paramIdx, volatile char* name);
// get parameter descriptor
void bfin_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc);
// get current module name
void bfin_get_module_name(volatile char* buf);

/// send some dummy bytes over SPI for no god damn fucking reason
void bfin_hack(U8 num);

#endif // header guard
