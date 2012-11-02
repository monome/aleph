/* memory.h
   avr32
   aleph

   a hacky sort of interface to explicitly allocate regions of SDRAM
   like malloc() without the free() !
*/


// asf
#include "print_funcs.h"
#include "sdramc.h"
#include "usart.h"

// aleph
#include "conf_aleph.h"
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

/*
// sdram test 
void sdram_test(void) {
  volatile unsigned long *sdram = SDRAM;
  u32 ret;
  u32 i;
  u8 y;

  for(i=0; i<5; i++) {
    sdram[i] = (i+1) * 11;
  }
  
  for(i=0; i<5; i++) {

    y = FONT_CHARH * i;
    //    ret = sdram[i];
    ret = sdram[i];

        usart_write_line(DBG_USART, "\r\n");
    print_dbg_ulong(ret);
    
    //    screen_line(0, y, "", 0x0);
    //    screen_int(0, y, ret, 0xf);
  }
  usart_write_line(DBG_USART, "\r\n");
}

*/

void sdram_test(u8 print) {
  volatile unsigned long * sdram = SDRAM;
  //  const u32 numWords = SDRAM_SIZE >> 2;
  const u32 numWords = 16;
  u32 i;
  u32 read;
  u32 errors = 0;
  if(print) {
    print_dbg("\r\nstarting SDRAM test (4096 words)...");
    // screen_line(0, 0, "starting SDRAM test (full)...", 0xf);
  }

  for(i=0; i<numWords; i++) {
    sdram[i] = i;
    if(print) {
      print_dbg("\r\n write: "); print_dbg_ulong(i);
    }
  }

  for(i=0; i<numWords; i++) {
    read = sdram[i];
    if(print) {
      print_dbg("\r\n read: "); print_dbg_ulong(read);
      print_dbg(" , last byte: "); print_dbg_ulong(read & 0xff);
    }
    if(i != read) {
      errors++;
    }
  }
  
  if(errors != 0) {
    if(print) {
      print_dbg("\r\nSDRAM test: corrupted words! :( \r\n");
      print_dbg_ulong(errors);
      print_dbg("\r\n");
    //    screen_line(0, 0, "SDRAM test: corrupted words! :(", 0xf);
    //    screen_int(0, FONT_CHARH, errors, 0xf);
    }
  } else {
    if(print) {
	print_dbg("\r\nSDRAM test completed OK!");
	//    screen_line(0, 0, "SDRAM test completed OK!", 0xf);
      }
  }
}
