//std
#include <string.h>
// ASF
#include "delay.h"
#include "flashc.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
// aleph-common
#include "module_common.h"
// aleph-bees
#include "scene.h"
#include "preset.h"
// aleph-avr32
#include "bfin.h"
#include "flash.h"
#include "memory.h"
#include "types.h"

//-----------------------------------
//----  define, typedef

/// 64K of blackfin executable storage in flash
#define LDR_FLASH_BYTES 0x10000

// value to write during first run
#define FIRSTRUN_INIT 0x76543210

// storage layout of default data in nonvolatile memory
typedef const struct {
  u32 firstRun;                // check for initialization
  u32 ldrSize;                 // size of stored LDR
  u8 ldrData[LDR_FLASH_BYTES]; // LDR data
  sceneData_t sceneData;       // scene data
} nvram_data_t;


//----------------------------------------
// ---- static vars
// NVRAM data structure located in the flash array.
__attribute__((__section__(".flash_nvram")))
static nvram_data_t flash_nvram_data;

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
  u32 i;

  print_dbg("\r\n init flash... ");

  // allocate bfin loader buf
  bfinLdrData = alloc_mem(BFIN_LDR_MAX_BYTES);
  
  print_dbg("\r\n bfinLdrData : @0x");
  print_dbg_hex( (u32)bfinLdrData );

  for(i=0; i<BFIN_LDR_MAX_BYTES; i++) {
    bfinLdrData[i] = 0;
  }

  if(flash_nvram_data.firstRun != FIRSTRUN_INIT) {
    print_dbg("\r\n writing firstrun, no bfin load");
    bfinLdrSize = 0;
    flashc_memset32((void*)&(flash_nvram_data.firstRun), FIRSTRUN_INIT, 4, true);
    // set size=0 so we won't attempt unitialized bfin load on next start
    flashc_memset32((void*)&(flash_nvram_data.ldrSize), 0x00000000, 4, true);
    return 1;
  } return 0;
  return 0;
}

// read default scene data to global buffer
void flash_read_scene(void) { 
  memcpy((void*)sceneData, (void*)&(flash_nvram_data.sceneData), sizeof(sceneData_t)); 
  //  scene_read_buf();
}

// write default scene data from global buffer
void flash_write_scene(void) { 
  //  scene_write_buf();
  flashc_memcpy( (void*)(&(flash_nvram_data.sceneData)), (void*)sceneData, sizeof(sceneData_t), true);
}

// read default blackfin
void flash_read_ldr(void) {
  bfinLdrSize = flash_nvram_data.ldrSize;
  print_dbg("\r\n read ldrSize from flash: ");
  print_dbg_ulong(bfinLdrSize);
  memcpy((void*)bfinLdrData, (void*)flash_nvram_data.ldrData, bfinLdrSize); 
  //  print_flash((u32)flash_nvram_data.ldrData, bfinLdrSize);
}

// write default blackfin
void flash_write_ldr(void) {
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
}
