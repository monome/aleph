#ifndef _FILES_H_
#define _FILES_H_

//#include "compiler.h"
#include "types.h"


// initialize filesystem navigation
extern void init_files(void);
// list all files (LS)
extern void files_list(void);

//// TEST:
//// load the first file found as a blackfin ELF
//extern void load_bfin_sdcard_test(void);

// create scenes dir if it doesn't exist
extern void files_check_scenes(void);
// create dsp dir if it doesn't exist
extern void files_check_dsp(void);

//----- dsp

// populate the dsp filelist with names and return count
extern void files_scan_dsp(void);
// return dsp filename for given index
extern const volatile char* files_get_dsp_name(u8 idx);
// return count of dsp files
extern u8 files_get_dsp_count(void);
// load a blackfin executable by index
extern void files_load_dsp(u8 idx);
// load a blackfin executable by filename
extern void files_load_dsp_name(const char* name);
// store dsp in flash, by index
extern void files_store_default_dsp(u8 idx);

//----- firmware
// return count of firmware files
extern u8 files_get_firmware_count(void);
// return firmware filename for given index
extern const volatile char* files_get_firmware_name(u8 idx);

// update avr32 firmware from binary file index
extern void files_write_firmware(u8 idx);
// update avr32 firmware from binary file path
extern void files_write_firmware_name(const char* name);

#endif // header guard 
