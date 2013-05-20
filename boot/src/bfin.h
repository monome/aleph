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

// blackfin HWAIT status
// extern volatile U8 hwait;

// 64k is max size of blackfin ldr file
#define BFIN_LDR_MAX_BYTES 0x10000 

// load bfin from RAM buffer
void bfin_load_buf(void);

// set a parameter
void bfin_set_param(U8 idx, fix16_t val);

// get number of parameters 
void bfin_get_num_params(volatile u32* num);
// get a parameter name
// void bfin_get_param_name(u16 paramIdx, volatile char* name);
// get parameter descriptor
void bfin_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc);
// get current module name
void bfin_get_module_name(volatile char* buf);

// clear and add params to ctl network
void bfin_report_params(void);


#endif // header guard
