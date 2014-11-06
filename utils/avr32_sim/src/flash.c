//std
#include <string.h>
// ASF
//#include "delay.h"
//#include "flashc.h"
//#include "power_clocks_lib.h"
#include "print_funcs.h"
// aleph-common
#include "module_common.h"
// aleph-avr32
#include "bfin.h"
#include "flash.h"
#include "memory.h"
#include "types.h"

//-----------------------------------
//----  define, typedef

// arbitrary magic number to write during first run
#define FIRSTRUN_MAGIC 0x76543210

// storage layout of default data in nonvolatile memory
typedef const struct {
  u32 firstRun;                // check for initialization
  u32 ldrSize;                 // size of stored LDR
  char ldrString[LDR_FLASH_STRING_LEN];
  u8 ldrData[LDR_FLASH_BYTES]; // LDR data
  // app-specific nonvolatile storage
  u8 appData[APP_FLASH_BYTES];
} nvram_data_t;

//----------------------------------------
// ---- static vars
// NVRAM data structure located in the flash array.
//__attribute__((__section__(".flash_nvram")))
nvram_data_t flash_nvram_data;

//--------------------------------
// ---- extern vars

// RAM buffer for avr32 firmware (.hex, actually)
volatile u8* fwBinData;
// size of avr32 firmware
volatile u32 fwBinSize = 0;

// RAM buffer for blackfin firmware (.ldr)
volatile u8 *bfinLdrData;
// size of bfin firmware
volatile u32 bfinLdrSize = 0;

//-------------------------------------------------------
// ---- function def

// intiailize (alloc mem, check/set firstrun bytes)
u8 init_flash() {
#if 1
#else
  u32 i;

  print_dbg("\r\n init flash... ");

  // allocate bfin loader buf
  ////////////////////
  /////////// TESTING
#if 0
  bfinLdrData = alloc_mem(BFIN_LDR_MAX_BYTES);
#else
  bfinLdrData = alloc_mem(BFIN_LDR_MAX_BYTES * 4);
#endif
  for(i=0; i<BFIN_LDR_MAX_BYTES; i++) { bfinLdrData[i] = 0; }

  if(flash_nvram_data.firstRun != FIRSTRUN_MAGIC) {
    // set size=0 so we won't attempt unitialized bfin load on next start
    bfinLdrSize = 0;
    flashc_memset32((void*)&(flash_nvram_data.ldrSize), 0x00000000, 4, true);
    // do this only after succesful app launch
    //    flashc_memset32((void*)&(flash_nvram_data.firstRun), FIRSTRUN_MAGIC, 4, true);
    return 1;
  } else { 
    // firstrun already happened
    return 0;
  }
#endif
}

// read default blackfin 
void flash_read_ldr(void) {
#if 1
#else
  bfinLdrSize = flash_nvram_data.ldrSize;
  print_dbg("\r\n read ldrSize from flash: ");
  print_dbg_ulong(bfinLdrSize);
  memcpy((void*)bfinLdrData, (void*)flash_nvram_data.ldrData, bfinLdrSize); 

    // TEST: print module data in RAM
#if 0
    for(u32 i = 0; i<bfinLdrSize; i += 4) {
      if((i % 16) == 0) {
	print_dbg("\r\n");
      }
      print_dbg(" 0x");
      print_dbg_hex(*((u32*)(bfinLdrData + i)));
    }
#endif

  
  //  print_flash((u32)flash_nvram_data.ldrData, bfinLdrSize);
#endif
}

// write default blackfin
void flash_write_ldr(void) {
#if 1
#else
  //  flashc_memset32((void*)&(flash_nvram_data.ldrSize), bfinLdrSize, 4, true);
  //  flashc_memcpy((void*)&(flash_nvram_data.ldrData), (const void*)bfinLdrData, bfinLdrSize, true);
  // seeing some missing pages, so try writing one page at a time
  u32 i;
  u32 nPages = bfinLdrSize / 0x200;
  u32 rem;
  const u8* pSrc;
  u8* pDst;
  // write size
  flashc_memset32((void*)&(flash_nvram_data.ldrSize), bfinLdrSize, 4, true);
  // write data 
  pSrc = (const void*)bfinLdrData;
  pDst = (void*)&(flash_nvram_data.ldrData);
  for(i=0; i<nPages; i++) {
    flashc_memcpy((void*)pDst, (const void*)pSrc, 0x200, true);
    pDst += 0x200;
    pSrc += 0x200;
    delay_ms(1);
  }
  // remaining bytes
  rem = bfinLdrSize - (nPages * 0x200);
  flashc_memcpy((void*)pDst, (const void*)pSrc, rem, true);
#endif
}

// read firstrun status
u8 flash_read_firstrun(void) {
#if 1
#else
  return (flash_nvram_data.firstRun == FIRSTRUN_MAGIC);
#endif
}

// write firstrun status
void flash_write_firstrun(void) {
#if 1
#else
  flashc_memset32((void*)&(flash_nvram_data.firstRun), FIRSTRUN_MAGIC, 4, true);
#endif
}

// clear firstrun status
void flash_clear_firstrun(void) {
#if 1
#else
  flashc_memset32((void*)&(flash_nvram_data.firstRun), 0x00000000, 4, true);
#endif
}

/// get pointer to application data in flash
void* flash_app_data(void) {
#if 1
#else
  return (void*)(&(flash_nvram_data.appData));
#endif
}
