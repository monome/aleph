/* memory.h
   avr32
   aleph

   a hacky sort of interface to explicitly allocate regions of SDRAM
   like malloc() without the free() !
 */


// asf
#include "print_funcs.h"
#include "sdramc.h"

// aleph
#include "types.h"
#include "memory.h"

// pointers to heap-ish
static heap_t pHeapStart;
static heap_t  pHeapEnd;
static u32 heapOffset;
// bytes in sdram hardware
static u32 heapSize;

// test sdram
static void sdram_test(void);

// setup heap
void init_mem(void) {

  heapSize = SDRAM_SIZE;
  pHeapStart = SDRAM;
  pHeapEnd = pHeapStart + heapSize;
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


// test sdram
static void sdram_test(void) {
  // test:
  u32 i, j, progress_inc;
  u32 noErrors=0;
  u8 tmp;

  // test:
  print_dbg("\x0CSDRAM size: ");
  print_dbg_ulong(SDRAM_SIZE >> 20);
  print_dbg(" MB\r\n");

  // Initialize the external SDRAM chip.

  // Determine the increment of SDRAM word address requiring an update of the
  // printed progression status.
  progress_inc = (heapSize + 50) / 100;
  
  // Fill the SDRAM with the test pattern.
  for (i = 0, j = 0; i < heapSize; i++)
  {
    if (i == j * progress_inc)
    {
       print_dbg("\rFilling SDRAM with test pattern: ");
      print_dbg_ulong(j++);
      print_dbg_char('%');
    }
    pHeapStart[i] = (u8)i;	
  }
   print_dbg("\rSDRAM filled with test pattern       \r\n");

  // Recover the test pattern from the SDRAM and verify it.
  for (i = 0, j = 0; i < heapSize; i++)
  {
    if (i == j * progress_inc)
    {
      print_dbg("\rRecovering test pattern from SDRAM: ");
      print_dbg_ulong(j++);
      print_dbg_char('%');
    }
    tmp = pHeapStart[i];
    if (tmp != (u8)i)
    {
      noErrors++;
    }
  }
  print_dbg("\rSDRAM tested: ");
  print_dbg_ulong(noErrors);
  print_dbg(" corrupted bytes(s)       \r\n");


}
