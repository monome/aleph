/* flash.h
   aleph-avr32

   internal flash read/write

*/


#ifndef __ALEPH_FLASH_H__
#define __ALEPH_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "filesystem.h"
#include "bfin.h"
#include "types.h"

// bytes available for application-specific storage
// (increase as needed)
#define APP_FLASH_BYTES 0x28000 

/// 64K of blackfin executable storage in flash
//// actually, the ldr itself can be bigger than the SRAM.
#define LDR_FLASH_BYTES BFIN_LDR_MAX_BYTES
// length of .ldr string identifier in flash
#define LDR_FLASH_STRING_LEN 64


//------------------------
// extern variables

// RAM buffer for blackfin firmware (.ldr)
extern volatile u8 *bfinLdrData;
// size of bfin firmware
extern volatile u32 bfinLdrSize;

//---------------------------------
// extern functions

// intialize (check/write firstrun bytes)
// return 1 if this is the first run, 0 otherwise
extern u8 init_flash(void); 

// read/write default blackfin loader file to/from global ram buffer 
extern void flash_read_ldr(void);
extern void flash_write_ldr(void);

// read firstrun status (returns 1 if firstrun is set)
extern u8 flash_read_firstrun(void);
// write firstrun status (TRUE)
extern void flash_write_firstrun(void);
// clear firstrun status (FALSE)
extern void flash_clear_firstrun(void);

// TODO:
// extern const char* flash_get_dsp_name(void);

// switch to intel-hex format...
// parse a hex record and write the contents to flash if appropriate
// return 1 if EOF, 0 otherwise
extern u8 flash_write_hex_record(u8* data);

/// get pointer to application data in flash
extern void* flash_app_data(void);

#ifdef __cplusplus
}
#endif

#endif // h guard
