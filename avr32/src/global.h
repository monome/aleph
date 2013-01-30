/* global.h
   aleph-avr32

   global application variables.
   mostly initialized in interrupts.c
   where first used.
*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

///// FIXME: the protected header should obviously not be a global inclusion.
#include "net_protected.h"

// app timer tick counter
extern volatile U64 tcTicks;
// screen refresh flag
extern volatile U8 refresh;
// encoder values
extern S32 encVal[4];

// FIXME: trying to find the seet spot for op memory pool by declaring the ctlnet external and looking for it in the memory map.
extern ctlnet_t ctlnet;

#endif
