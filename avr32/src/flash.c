//std
#include <string.h>
// ASF
#include "flashc.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
//// aleph-bees
#include "scene.h"
#include "preset.h"
// aleph-avr32
#include "bfin.h"
#include "flash.h"
#include "memory.h"
#include "types.h"


//## dbg
//#define SCENE_OFF
//##

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

// RAM buffer for avr32 firmware (.bin)
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

  // why?
  for(i=0; i<BFIN_LDR_MAX_BYTES; i++) {
    bfinLdrData[i] = 0;
  }

  if(flash_nvram_data.firstRun != FIRSTRUN_INIT) {
    print_dbg("\r\n writing firstrun, no bfin load");
    bfinLdrSize = 0;
    flashc_memset32((void*)&(flash_nvram_data.firstRun), FIRSTRUN_INIT, 4, true);
    return 1;
  } return 0;
  return 0;
}

// read default scene data to pointer
void flash_read_scene(sceneData_t* sceneData) {
  memcpy((void*)sceneData, (void*)&(flash_nvram_data.sceneData), sizeof(sceneData_t)); 
  //  scene_read_buf();
}

// write default scene data from pointer
void flash_write_scene(sceneData_t* sceneData) {
  //  scene_write_buf();
  flashc_memcpy( (void*)(&(flash_nvram_data.sceneData)), (void*)sceneData, sizeof(sceneData_t), true);
}

// read default blackfin
void flash_read_ldr(void) {
  bfinLdrSize = flash_nvram_data.ldrSize;
  memcpy((void*)bfinLdrData, (void*)flash_nvram_data.ldrData, bfinLdrSize); 
}

// write default blackfin
void flash_write_ldr(void) {
  flashc_memset32((void*)&(flash_nvram_data.ldrSize), bfinLdrSize, 4, true);
  flashc_memcpy((void*)&(flash_nvram_data.ldrData), (const void*)bfinLdrData, bfinLdrSize, true);
}

/* static u32 flashoff =0x80000000; */
/* static void print_flash(void) { */
/*   u32 i, j; */
/*   u32 b, boff; */
/*   print_dbg("\r\n"); */
/*   print_dbg_hex(flashoff); */
/*   print_dbg(" : "); */
/*   for(j=0; j<8; j++) { */
/*     b = 0; */
/*     boff = 24; */
/*     for(i=0; i<4; i++) { */
/*       b |= ( *(u8*)flashoff ) << boff; */
/*       flashoff++; */
/*       boff -= 8; */
/*     } */
/*     print_dbg_hex(b); */
/*     print_dbg(" "); */
/*   } */
/* } */
