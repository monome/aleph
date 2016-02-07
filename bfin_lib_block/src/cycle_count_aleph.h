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

//! start a cycle counter by storing current cycles in supplied long int
/*
#define START_CYCLE_COUNT( COUNT ) \
  asm volatile ("%0 = CYCLES; %H0 = CYCLES2;" : "=d" (COUNT))
*/

//! take a cycle count and store the difference in the first argument,
//! taking the second argument as the starting cycle count
//! factor in the overhead of the calculation
/*
#define STOP_CYCLE_COUNT( COUNT, START ) \
  asm volatile ("%0 = CYCLES; %H0 = CYCLES;" : "=d" (COUNT)); \
  (COUNT) = (COUNT) - (START) - 4
*/

#endif // h guard
