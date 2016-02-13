/* global.h
   aleph-avr32

   global application variables.
   mostly initialized in interrupts.c
   where first used.
*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
//#include "net_protected.h"
// extern extlnet_t* ctlnet;

//---- global buffers

// RAM buffer for blackfin firmware
extern volatile u8* bfinLdrData;
// size of bfin firmware
extern volatile u32 bfinLdrSize;

// app timer tick counter
extern volatile u64 tcTicks;
// screen refresh flag
extern volatile u8 refresh;
// current encoder values
extern s32 encVal[4];

// FTDI connection flag
extern u8 ftdiPlug;

#ifdef __cplusplus
}
#endif

#endif
