//std
#include <string.h>
// ASF
#include "flashc.h"
//#include "print_funcs.h"
#include "power_clocks_lib.h"
#include "wdt.h"
// aleph-bees
// aleph-avr32
#include "bfin.h"
#include "flash.h"
#include "memory.h"
#include "types.h"
// boot
#include "parse_hex.h"
#include "util.h"

//-----------------------------------
//----  define, typedef

/// 64K of blackfin executable storage in flash
#define LDR_FLASH_BYTES 0x10000
// value to write during first run
#define FIRSTRUN_INIT 0x76543210
// size of RAM buffer for firmware image (.elf)
#define FIRMWARE_MAX_BYTES 0x80000 // 512K

// storage layout of default data in nonvolatile memory
typedef const struct {
  u32 firstRun;  // check for initialization
  u32 ldrSize;   // size of default blackfin loader
  u8 ldrData[LDR_FLASH_BYTES];
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

  // allocate .elf buffer
  fwBinData = alloc_mem(FIRMWARE_MAX_BYTES);
  bfinLdrData = alloc_mem(BFIN_LDR_MAX_BYTES);

  for(i=0; i<BFIN_LDR_MAX_BYTES; i++) {
    bfinLdrData[i] = 0;
  }

  for(i=0; i<FIRMWARE_MAX_BYTES; i++) {
    fwBinData[i] = 0;
  }
  if(flash_nvram_data.firstRun != FIRSTRUN_INIT) {
    // print_dbg("\r\n writing firstrun, no bfin load");
    bfinLdrSize = 0;
    flashc_memset32((void*)&flash_nvram_data.firstRun, FIRSTRUN_INIT, 4, true);
    return 1;
  } return 0;
}

// clear firstrun status
void flash_clear_firstrun(void) {
  flashc_memset32((void*)&(flash_nvram_data.firstRun), 0x00000000, 4, true);
}

// read default blackfin
void flash_read_ldr(void) {
  bfinLdrSize = flash_nvram_data.ldrSize;
  memcpy((void*)bfinLdrData, (void*)flash_nvram_data.ldrData, bfinLdrSize); 
}

// write default blackfin
/* void flash_write_ldr(void) { */
/*   flashc_memset32((void*)&(flash_nvram_data.ldrSize), bfinLdrSize, 4, true); */
/*   flashc_memcpy((void*)&(flash_nvram_data.ldrData), (const void*)bfinLdrData, bfinLdrSize, true); */
/* } */

/* static u32 flashoff =0x80000000; */
/* static void print_flash(void) { */
/*   u32 i, j; */
/*   u32 b, boff; */
/*   // print_dbg("\r\n"); */
/*   // print_dbg_hex(flashoff); */
/*   // print_dbg(" : "); */
/*   for(j=0; j<8; j++) { */
/*     b = 0; */
/*     boff = 24; */
/*     for(i=0; i<4; i++) { */
/*       b |= ( *(u8*)flashoff ) << boff; */
/*       flashoff++; */
/*       boff -= 8; */
/*     } */
/*     // print_dbg_hex(b); */
/*     // print_dbg(" "); */
/*   } */
/* } */

// parse a hex record and write the contents to flash if appropriate
// return 1 if EOF, 0 otherwise
extern u8 flash_write_hex_record(u8* data) {
  static hexRecord_t rec;
  static u32 addrOff = 0;
  static u32 dst;
  static char hexBuf[9] = {0,0,0,0,0,0,0,0,0};
  int err;
  err = parse_raw_hex_record(data, &rec);
  if(err) {
    // print_dbg("\r\n failure parsing hex record: \r\n");
    // print_dbg((const char*)data);

    screen_line(0, 6, "WARNING:", 0xf);
    screen_line(0, 7, "error parsing hex record!", 0xf);
    screen_refresh();

  } else {
    switch(rec.type) {
    case HEX_EXT_LINEAR_ADDRESS:
      addrOff = rec.address;
      break;
    case HEX_DATA:
      /* // print_dbg("\r\n writing firmware to flash at address: "); */
      /* // print_dbg_hex(addrOff + rec.address); */
      dst = addrOff + rec.address;

      /// TEST!!
            if(dst < FIRMWARE_FLASH_ADDRESS) {
      //      if(0) {


	// don't allow writes to anything below the runtime location!
	// this is where the bootloader lives!
	// app data goes at the end of flash.
	screen_line(0, 6, "WARNING:", 0xf);
	screen_line(0, 7, "scary address! ", 0xf);
	uint_to_hex_ascii( hexBuf, dst);
	screen_line(64, 7, hexBuf, 0xf);
      } else {
	flashc_memcpy( (void*)(dst), rec.data, rec.count, 1);
      }
      break;
    default:
      ;;
    }
  }
  return 0;
}
