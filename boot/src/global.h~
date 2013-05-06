/* global.h
   aleph-avr32

   global application variables.
   mostly initialized in interrupts.c
   where first used.
*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_


///// FIXME: the protected header should obviously not be a global inclusion. see below.
#include "net_protected.h"

// FIXME: trying to find a sweet spot for op memory pool by declaring the ctlnet external and looking for it in the memory map.
/// make it static to main.c when done
extern ctlnet_t ctlnet;

// app timer tick counter
extern volatile U64 tcTicks;
// screen refresh flag
extern volatile U8 refresh;
// current encoder values
extern S32 encVal[4];

#endif
