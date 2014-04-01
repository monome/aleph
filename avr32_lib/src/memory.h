/* memory.h
   avr32
   aleph

   a custom interface to allocate external SDRAM
   mapped to malloc/free now that the heap == SDRAM
 */ 

#ifndef _ALEPH_MEM_H_
#define _ALEPH_MEM_H_

#include "types.h"

#define ALLOC_FAIL 0xffffffff

// heap memory type
typedef volatile u8 * heap_t;

// setup heap
extern void init_mem(void);
// allocate and return pointer
extern heap_t alloc_mem(u32 bytes);
// free pointer
extern void free_mem(heap_t);
// test sram
extern void sram_test(u32 sizeDiv, u32 offset);
extern void test_malloc( void );
#endif
