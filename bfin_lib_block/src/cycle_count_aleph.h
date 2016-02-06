/*
  toolchain cycle-count headers seem to not have been totally ported from VDSP, maybe?

in any case, there are some strange keywords in there. so putting the relevant macros here instead.
 */

#ifndef _ALEPH_BFIN_CYCLE_COUNT_H_
#define _ALEPH_BFIN_CYCLE_COUNT_H_

#include "audio.h"
#include "module_custom.h"
#include "types.h"

// theoretically we should be able to get this?
// #define __PROCESSOR_SPEED__ 594000000
// but at the moment we have this:
#define PROCESSOR_CLOCK_HZ 512000000


typedef volatile unsigned long cycle_t;

#define COMPUTE_CPU_USE( CYCLES ) ( (u32)((u64)0x7fffffff * (u64)(CYCLES * AUDIO_SAMPLERATE) / (u64)(MODULE_BLOCKSIZE * PROCESSOR_CLOCK_HZ)) )


//! start a cycle counter by storing current cycles in supplied integer
#define START_CYCLE_COUNT( _CURR_COUNT )          \
                             __asm volatile ("r2 = CYCLES;  \n"  \
					     "r1 = CYCLES2; \n"  \
					     "[%0]   = r2;  \n"		\
					     "[%0+4] = r1;  \n"		\
					     : : "p" (&(_CURR_COUNT))	\
					     : "R1", "R2" );


//! take a cycle count and store the difference in the first argument,
//! taking the second argument as the starting cycle count
//! factor in the overhead of the calculation
#define STOP_CYCLE_COUNT( _CURR_COUNT, _START_COUNT ) \
                             __asm volatile ("r2 = CYCLES;  \n"  \
					     "r1 = CYCLES2; \n"	    \
					     "[%0]   = r2;  \n"	    \
					     "[%0+4] = r1;  \n"		\
					     : : "p" (&(_CURR_COUNT))	\
					     : "R1", "R2" );		\
			     (_CURR_COUNT) = (_CURR_COUNT) - (_START_COUNT); \
			     (_CURR_COUNT) -= (cycle_t)4;

#endif // h guard
