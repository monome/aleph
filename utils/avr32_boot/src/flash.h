/* flash.h
   aleph-avr32

   internal flash read/write

*/


#ifndef __ALEPH_FLASH_H__
#define __ALEPH_FLASH_H__

#include "screen.h"

/// target address of firmware in internal flash
/// NOTE: this is hardcoded ASM in main()!
/// also it is hardcoded in application linker scripts!
/// DO NOT CHANGE. EVER.
#define FIRMWARE_FLASH_ADDRESS 0x80008000

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

// clear firstrun field
extern void flash_clear_firstrun(void);

// read/write default blackfin loader file to/from global ram buffer 
extern void flash_read_ldr(void);
//extern void flash_write_ldr(void);

// write avr32 firmware
// extern void flash_write_firmware(void);

// switch to intel-hex format...
// parse a hex record and write the contents to flash if appropriate
// return 1 if EOF, 0 otherwise
extern u8 flash_write_hex_record(u8* data);


#endif // h guard
