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

// sdram test 
/*
void sdram_test(void) {
  volatile unsigned long *sdram = SDRAM;
  u32 ret;
  u32 i;
  u8 y;

  // test sdram chip configuration
  print_dbg("\r\nsdram bank bits: ");
  print_dbg_ulong(SDRAM_BANK_BITS);
  print_dbg("\r\nsdram row bits: ");
  print_dbg_ulong(SDRAM_ROW_BITS);
  print_dbg("\r\nsdram col bits: ");
  print_dbg_ulong(SDRAM_COL_BITS);
  print_dbg("\r\nsdram CAS: ");
  print_dbg_ulong(SDRAM_CAS);
  print_dbg("\r\nsdram TWR: ");
  print_dbg_ulong(SDRAM_TWR);
  print_dbg("\r\nsdram TRC: ");
  print_dbg_ulong(SDRAM_TRC);
  print_dbg("\r\nsdram TRP: ");
  print_dbg_ulong(SDRAM_TRP);
  print_dbg("\r\nsdram TRCD: ");
  print_dbg_ulong(SDRAM_TRCD);
  print_dbg("\r\nsdram TRAS: ");
  print_dbg_ulong(SDRAM_TRAS);
  print_dbg("\r\nsdram TXSR: ");
  print_dbg_ulong(SDRAM_TXSR);
  print_dbg("\r\nsdram TR: ");
  print_dbg_ulong(SDRAM_TR);
  print_dbg("\r\nsdram TRFC: ");
  print_dbg_ulong(SDRAM_TRFC);
  print_dbg("\r\nsdram TMRD: ");
  print_dbg_ulong(SDRAM_TMRD);
  print_dbg("\r\nsdram stable clock init delay: ");
  print_dbg_ulong(SDRAM_STABLE_CLOCK_INIT_DELAY);
  print_dbg("\r\nsdram auto refresh: ");
  print_dbg_ulong(SDRAM_INIT_AUTO_REFRESH_COUNT);


  for(i=0; i<5; i++) {
    sdram[i] = (i+1) * 11;
  }
  
  for(i=0; i<5; i++) {
    
    y = FONT_CHARH * i;
    ret = sdram[i];

    print_dbg("\r\n");
    print_dbg_ulong(ret);
    
    screen_line(0, y, "", 0x0);
    screen_int(0, y, ret, 0xf);
  }
  print_dbg("\r\n");
}
*/

volatile unsigned long * sdram = SDRAM;

//// make things a little weirder...
static u32 byte_swap(u32 x) {
  static union { u32 i; u8 b[4]; } xu;
  static u8 tmp;
  xu.i = x;
  tmp = xu.b[0];
  xu.b[0] = xu.b[3];
  xu.b[3] = tmp;
  tmp = xu.b[1];
  xu.b[1] = xu.b[2];
  xu.b[2] = tmp;

  return xu.i;
}

static void sdram_test_write(unsigned long addr, unsigned long val) {
  sdram[addr] = byte_swap(val);
  //  print_dbg("\r\n write: "); print_dbg_ulong(val);
  /*
  x = screen_line(0, l, "write: ", 0xf);
  screen_int(x, l, val, 0xf);    
  */
}

static u32 sdram_test_read(unsigned long addr, unsigned long i) {
  unsigned long val, exp;
  static char buf[64];
  val = sdram[addr];
  exp = byte_swap(i);
  snprintf(buf, 64,
	   "Read 0x%08lx @ 0x%08lx, expected 0x%08lx", val, addr, exp);
  print_dbg(buf);
  if(exp == val) { print_dbg("  OK!"); }
  print_dbg("\n\r");
  /*
  x = screen_line(64, l, "read: ", 0xf);
  x = screen_int(x, l, val, 0xf);   
  x = screen_line(x, l, " : ", 0xf);
  x = screen_int(x, l, val & 0xf, 0xf);   
  */
  return (exp == val);
}



void sdram_test(u8 print) {
  const u32 numWords = SDRAM_SIZE >> 2;
  unsigned long i;
  //  unsigned long tmp;
  u32 errors = 0;
  
  if(print) {
    print_dbg("\r\nstarting SDRAM test...\n");
  }

  for(i=0; i<numWords; i++) {
    sdram_test_write(i, i);
  }

  for(i=0; i<numWords; i++) {
    if( sdram_test_read(i, i) ) {
      ;;
    } else {
      errors++;
    }

  }
  
  if(errors != 0) {
    if(print) {
      print_dbg("\r\nSDRAM test: corrupted words! :( \r\n");
      print_dbg_ulong(errors);
      print_dbg("\r\n");
      screen_line(0, 0, "SDRAM test: corrupted words! :(", 0xf);
      screen_int(0, 1, errors, 0xf);
    }
  } else {
    if(print) {
      print_dbg("\r\nSDRAM test completed OK!");
      screen_line(0, 7, "SDRAM test completed OK!", 0xf);
    }
  }
}


