/* global.h
   aleph-avr32

   global application variables.
   mostly initialized in interrupts.c
   where first used.
*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_


// app timer tick counter
extern volatile U64 tcTicks;
// screen refresh flag
extern volatile U8 refresh;
// current encoder values
extern S32 encVal[4];

#endif
