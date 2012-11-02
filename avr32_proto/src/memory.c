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

static volatile unsigned long * sdram = SDRAM;

static void sdram_test_write(u8 addr, u32 val, u8 l) {
  static u8 x;
  sdram[addr] = val;
  print_dbg("\r\n write: "); print_dbg_ulong(val);
  x = screen_line(0, l, "write: ", 0xf);
  screen_int(x, l, val, 0xf);    
}

static u32 sdram_test_read(u8 addr, u8 l) {
  static u8 x;
  u32 val;
  val = sdram[addr];
  print_dbg("\r\n read: "); print_dbg_ulong(val);
  x = screen_line(64, l, "read: ", 0xf);
  x = screen_int(x, l, val, 0xf);   
  x = screen_line(x, l, " : ", 0xf);
  x = screen_int(x, l, val & 0xf, 0xf);   
  return val;
}



void sdram_test(u8 print) {
  //  const u32 numWords = SDRAM_SIZE >> 2;
  const u32 numWords = 16;
  u32 i;
  u32 read[16];
  u32 errors = 0;
  if(print) {
    print_dbg("\r\nstarting SDRAM test (4096 words)...");
  }

  for(i=0; i<numWords; i++) {
    sdram_test_write(i, i, i);
  }

  for(i=0; i<numWords; i++) {
    read[i] = sdram_test_read(i, i); 
    if(i != read[i]) {
      errors++;
    }
  }
  
  if(errors != 0) {
    if(print) {
      print_dbg("\r\nSDRAM test: corrupted words! :( \r\n");
      print_dbg_ulong(errors);
      print_dbg("\r\n");
      screen_line(0, 0, "SDRAM test: corrupted words! :(", 0xf);
      screen_int(0, 7, errors, 0xf);
    }
  } else {
    if(print) {
      print_dbg("\r\nSDRAM test completed OK!");
      screen_line(0, 7, "SDRAM test completed OK!", 0xf);
    }
  }
}


