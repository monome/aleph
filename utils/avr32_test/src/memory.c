/* memory.h
   avr32
   aleph

   a hacky sort of interface to explicitly allocate regions of SRAM
   like malloc() without the free() !
*/

//// test
#include <stdio.h>

// asf
#include "delay.h"
#include "print_funcs.h"
#include "smc.h"

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
static u32 heapOffset;
// bytes in sram hardware
static u32 heapSize;


// setup heap-ish
void init_mem(void) {
  heapSize = SRAM_SIZE;
  pHeapStart = (heap_t)SRAM;
  pHeapEnd = (heap_t)((u32)pHeapStart + heapSize);
  heapOffset = 0;
}

// allocate and return pointer
heap_t alloc_mem(u32 bytes) {
  print_dbg("\r\n allocating memory, bytes: 0x");
  print_dbg_hex(bytes);

  print_dbg("\r\n location: 0x");
  print_dbg_hex(heapOffset);

  u32 tmp = heapOffset + bytes;
  u8 mtmp = tmp % 4;
  heap_t ret;
  // align to 4 bytes
  if ( mtmp != 0) {
    tmp += ( 4 - mtmp );
  }
  if (tmp < heapSize) {
    heapOffset = tmp;
    ret = pHeapStart + heapOffset;
  } else {
    ret = (heap_t)ALLOC_FAIL;
  }
  return ret;
}

// memory test routine
void sram_test(void) {

  unsigned long sram_size, progress_inc, i, j, tmp, noErrors = 0;
  volatile unsigned long *sram = SRAM;
  
  sram_size = SRAM_SIZE >> 2;
  //  sram_size = numBytes >> 2; // count of 32-bit words
  print_dbg("\r\n SRAM size (words): ");
  print_dbg_ulong(sram_size);
  print_dbg("\r\n");
  
  delay_ms(1);
  
  progress_inc = (sram_size + 50) / 100; // words per progress report

  // Fill the SRAM with the test pattern.
  for (i = 0, j = 0; i < sram_size; i++) {
    if (i == j * progress_inc) {
      //      print_dbg("\rFilling SRAM with test pattern: ");
      //      print_dbg_ulong(j++);
      //      print_dbg_char('%');
    }
    sram[i] = i;
    //    print_dbg("\r\n write: 0x");
    //    print_dbg_hex(i);
  }
  print_dbg("\rSRAM filled with test pattern       \r\n");

  // Recover the test pattern from the SRAM and verify it.
  for (i = 0, j = 0; i < sram_size; i++) {
    //    if (i == j * progress_inc) {
    //      print_dbg("\rRecovering test pattern from SRAM: ");
    //      print_dbg_ulong(j++);
    //      print_dbg_char('%');
    //    }
    tmp = sram[i];
    // print_dbg(" \r\n word index/value: 0x");
    // print_dbg_hex(i); 
    // print_dbg(" ; read: 0x");
    // print_dbg_hex(tmp);
    if (tmp != i) {
      // print_dbg(" ERROR");
      noErrors++;
    }
  }
  print_dbg("\rSRAM tested: ");
  print_dbg_ulong(noErrors);
  print_dbg(" corrupted word(s)       \r\n");
}
