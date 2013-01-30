/* memory.h
   avr32
   aleph

   a hacky sort of interface to explicitly allocate regions of SDRAM
   like malloc() without the free() !
*/

//// test
#include <stdio.h>

// asf
#include "print_funcs.h"
#include "sdramc.h"

// aleph
#include "screen.h"
#include "types.h"
#include "memory.h"

// pointers to heap-ish
static heap_t pHeapStart;
static heap_t  pHeapEnd;
static u32 heapOffset;
// bytes in sdram hardware
static u32 heapSize;


// setup heap
void init_mem(void) {
  heapSize = SDRAM_SIZE;
  pHeapStart = (heap_t)SDRAM;
  pHeapEnd = (heap_t)((u32)pHeapStart + heapSize);
  heapOffset = 0;
}

// allocate and return pointer
heap_t alloc_mem(u32 bytes) {
  u32 tmp = heapOffset + bytes;
  u8 mtmp = tmp % 4;
  heap_t ret;
  // align to 4 bytes
  if ( mtmp != 0) {
    tmp += ( 4 - mtmp );
  }
  if (tmp < heapSize) {
    ret = pHeapStart;
    heapOffset = tmp;
  } else {
    ret = (heap_t)ALLOC_FAIL;
  }
  return ret;
}

// memory test routine
void sdram_test(u32 numBytes, u32 offset) {
  unsigned long sdram_size, progress_inc, i, j, tmp, noErrors = 0;
  volatile unsigned long *sdram = SDRAM + offset;
  
  //sdram_size = SDRAM_SIZE >> 2;
  sdram_size = numBytes >> 2; // count of 32-bit words
  print_dbg("\x0CSDRAM size: ");
  print_dbg_ulong(sdram_size >> 18);
  print_dbg(" MB\r\n");
  
  progress_inc = (sdram_size + 50) / 100; // words per progress report

  // Fill the SDRAM with the test pattern.
  for (i = 0, j = 0; i < sdram_size; i++) {
    if (i == j * progress_inc) {
      print_dbg("\rFilling SDRAM with test pattern: ");
      print_dbg_ulong(j++);
      print_dbg_char('%');
    }
    sdram[i] = i;
  }
  print_dbg("\rSDRAM filled with test pattern       \r\n");

  // Recover the test pattern from the SDRAM and verify it.
  for (i = 0, j = 0; i < sdram_size; i++) {
    if (i == j * progress_inc) {
      print_dbg("\rRecovering test pattern from SDRAM: ");
      print_dbg_ulong(j++);
      print_dbg_char('%');
    }
    tmp = sdram[i];
    if (tmp != i) {
      noErrors++;
    }
  }
  print_dbg("\rSDRAM tested: ");
  print_dbg_ulong(noErrors);
  print_dbg(" corrupted word(s)       \r\n");
}
