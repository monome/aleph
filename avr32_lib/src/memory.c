/* memory.h
   avr32
   aleph

   custom interface to allocate regions of SRAM
   now just calls into malloc/free.
*/

//// test
#include <stdio.h>

// asf
#include "print_funcs.h"

// aleph
#include "screen.h"
#include "types.h"
#include "memory.h"


// setup heap-ish
void init_mem(void) {
  // this space left intentionally empty 
}

// allocate and return pointer
heap_t alloc_mem(u32 bytes) {
  void* mem_ptr;

  print_dbg("\r\n >>> alloc_mem(), requested bytes: 0x");
  print_dbg_hex(bytes);

  mem_ptr = malloc(bytes);
  if (mem_ptr == 0) {
     print_dbg("\r\n memory allocation failed!");
  }

  print_dbg("\r\n memory allocation result: 0x");
  print_dbg_hex((u32)mem_ptr);

  return (heap_t)mem_ptr;
}

void free_mem(heap_t mem_ptr)
{
	free((void *)mem_ptr);
}

// memory test routine
void sram_test(u32 numBytes, u32 offset) {

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
}



// Number of dynamic allocation to perform
#define EXTSDRAM_EXAMPLE_NB_MALLOC      4

// The dynamically allocated buffer size.
#define EXTSDRAM_EXAMPLE_MALLOC_SIZE    128

// The pattern written to the dynamically allocated buffer.
#define EXTSDRAM_EXAMPLE_PATTERN        0xCAFEBABE

// The size of the huge buffer.
#define EXTSDRAM_EXAMPLE_HUGEBUFF_SIZE  16385

// malloc() tests
void test_malloc( void )
{

  //##################################################
  //## II. Dynamic allocation in external SDRAM tests.
  //##################################################
  int *pBuf;          // Pointer on malloc'ed buffer
  int *pTempo;        // Temporary pointer
  int i,j;            // counters.
  int iTempoValue;    // Temporary value
  U8  u8NbErrors = 0; // Number of read/write errors
  int *au32StoreMallocPtr[EXTSDRAM_EXAMPLE_NB_MALLOC];

  for(i=0; i<EXTSDRAM_EXAMPLE_NB_MALLOC; i++)
  {
    // II.1 Malloc a buffer of EXTSDRAM_EXAMPLE_MALLOC_SIZE int.
    pBuf = (int *)malloc(EXTSDRAM_EXAMPLE_MALLOC_SIZE*sizeof(int));
    au32StoreMallocPtr[i] = pBuf;
    if (pBuf)
    {   // Malloc succeeded.
      // Print the address of the allocated buffer to USART1.
      print_dbg("\r\nMalloc OK at address: 0x");
      print_dbg_hex((int) pBuf);

      // II.2 Fill this buffer with a pattern.
      print_dbg("\r\nWriting the pattern 0x");
      print_dbg_hex(EXTSDRAM_EXAMPLE_PATTERN);
      print_dbg(" to the dynamically allocated buffer...");
      pTempo = pBuf;
      j = EXTSDRAM_EXAMPLE_MALLOC_SIZE;
      while(j--)
        *pTempo++ = EXTSDRAM_EXAMPLE_PATTERN;

      // II.3 Check that the pattern was correctly written.
      print_dbg("\r\nChecking...");
      u8NbErrors = 0;
      pTempo--;
      while(pTempo >= pBuf)
      {
        iTempoValue = *pTempo--;
        if(EXTSDRAM_EXAMPLE_PATTERN != iTempoValue)
        {
          u8NbErrors++;
          print_dbg("\r\nInvalid value read: 0x");
          print_dbg_hex(iTempoValue);
        }
      }
      print_dbg("\r\nNumber of read/write errors: ");
      print_dbg_ulong(u8NbErrors);
      print_dbg("\r\n");
    }
    else
    {   // Should never happen!
      print_dbg("\r\nMalloc failed\r\n");
    }
  }
  // II.4 Free the previously allocated buffers.
  for(i=0; i<EXTSDRAM_EXAMPLE_NB_MALLOC; i++)
  {
    pBuf = au32StoreMallocPtr[i];
    if(NULL != pBuf)
      free(pBuf);
  }
}

