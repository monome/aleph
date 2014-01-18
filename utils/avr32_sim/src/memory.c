/* memory.h
   avr32
   aleph

   a hacky sort of interface to explicitly allocate regions of SRAM
   like malloc() without the free() !
*/

//// test
#include <stdio.h>

// asf
#include "print_funcs.h"
//#include "smc.h"

// aleph
#include "screen.h"
#include "types.h"
#include "memory.h"


//  SRAM base address
#define SRAM              ((void *)AVR32_EBI_CS1_ADDRESS)
// SRAM size.
#define SRAM_SIZE         (1 << smc_get_cs_size(1))


// pointers to heap-ish
static heap_t pHeapStart;
static heap_t  pHeapEnd;
static u32 heapOffset = 0;
// bytes in sram hardware
static u32 heapSize;


// setup heap-ish
void init_mem(void) {
#if 1
#else
  heapSize = SRAM_SIZE;
  pHeapStart = (heap_t)SRAM;
  pHeapEnd = (heap_t)((u32)pHeapStart + heapSize);
  heapOffset = 0;

  print_dbg("\r\n SRAM size: 0x");
  print_dbg_hex(heapSize);

#endif
}

// allocate and return pointer
heap_t alloc_mem(u32 bytes) {
#if 1

  return malloc(bytes);

#else
  //  print_dbg("\r\n allocating memory, bytes: 0x");
  //  print_dbg_hex(bytes);

  //  print_dbg("\r\n location: 0x");
  //  print_dbg_hex(heapOffset);

  heap_t ret = pHeapStart + heapOffset;

  u32 tmp = heapOffset + bytes;
  u8 mtmp = tmp % 4;

  print_dbg("\r\n >>> alloc_mem(), requested bytes: 0x");
  print_dbg_hex(bytes);
  print_dbg(" , location: 0x");
  print_dbg_hex(ret);

  // align to 4 bytes
  if ( mtmp != 0) {
    tmp += ( 4 - mtmp );
  }
  if (tmp < heapSize) {
    heapOffset = tmp;
    //    ret = pHeapStart + heapOffset;
  } else {
    print_dbg("\r\n memory allocation failed!");
    ret = (heap_t)ALLOC_FAIL;
  }
  return ret;
#endif
}

// memory test routine
void sram_test(u32 numBytes, u32 offset) {
#if 1
#else

  /* unsigned long sram_size, progress_inc, i, j, tmp, noErrors = 0; */
  /* volatile unsigned long *sram = SRAM + offset; */
  
  /* //sram_size = SRAM_SIZE >> 2; */
  /* sram_size = numBytes >> 2; // count of 32-bit words */
  /* print_dbg("\x0CSRAM size: "); */
  /* print_dbg_ulong(sram_size >> 18); */
  /* print_dbg(" MB\r\n"); */
  
  /* progress_inc = (sram_size + 50) / 100; // words per progress report */

  /* // Fill the SRAM with the test pattern. */
  /* for (i = 0, j = 0; i < sram_size; i++) { */
  /*   if (i == j * progress_inc) { */
  /*     print_dbg("\rFilling SRAM with test pattern: "); */
  /*     print_dbg_ulong(j++); */
  /*     print_dbg_char('%'); */
  /*   } */
  /*   sram[i] = i; */
  /* } */
  /* print_dbg("\rSRAM filled with test pattern       \r\n"); */

  /* // Recover the test pattern from the SRAM and verify it. */
  /* for (i = 0, j = 0; i < sram_size; i++) { */
  /*   if (i == j * progress_inc) { */
  /*     print_dbg("\rRecovering test pattern from SRAM: "); */
  /*     print_dbg_ulong(j++); */
  /*     print_dbg_char('%'); */
  /*   } */
  /*   tmp = sram[i]; */
  /*   if (tmp != i) { */
  /*     noErrors++; */
  /*   } */
  /* } */
  /* print_dbg("\rSRAM tested: "); */
  /* print_dbg_ulong(noErrors); */
  /* print_dbg(" corrupted word(s)       \r\n"); */
#endif
}
