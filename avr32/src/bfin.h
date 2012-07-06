/* bfin.h
   aleph-avr32

   routines to communicate with the bf533 DSP
*/

#ifndef _BFIN_H_
#define _BFIN_H_

#include "compiler.h"

// blackfin HWAIT status
extern volatile U8 hwait;

// load an .ldr file over SPI
void bfin_load(U32 size, char * data);
// perform a parameter transaction over SPI
void bfin_param(U32 idx, F32 val);

#endif // header guard
