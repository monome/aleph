/* memory.h
   avr32
   aleph

   a hacky sort of interface to explicitly allocate consecutive regions of SDRAM
 */ 

#ifndef _ALEPH_MEM_H_
#define _ALEPH_MEM_H_

#include "types.h"

#define ALLOC_FAIL 0

// heap memory type
typedef volatile u8 * heap_t;

// setup heap
extern void init_mem(void);
// allocate and return pointer
extern heap_t alloc_mem(u32 bytes);
// test sram
extern void sram_test(u32 sizeDiv, u32 offset);

#endif
