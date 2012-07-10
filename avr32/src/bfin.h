/* bfin.h
   aleph-avr32

   routines to communicate with the bf533 DSP
*/

#ifndef _BFIN_H_
#define _BFIN_H_

#include "compiler.h"

// blackfin HWAIT status
extern volatile U8 hwait;

// load a blackfin executable
void bfin_load(U32 size, char * data);
// set a parameter
void bfin_set_param(U32 idx, F32 val);
// get number of parameters 
u16 bfin_get_num_params();
// get a parameter name
void bfin_get_param_name(u16 paramIdx, char * name);
// get parameter descriptor
void bfin_get_param_name(u16 paramIdx, ParamDesc* pDesc);
#endif // header guard
