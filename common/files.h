#ifndef _FILES_H_
#define _FILES_H_

//#include "compiler.h"
#include "types.h"
// initialize filesystem navigation
extern void init_files(void);
// list all files (LS)
extern void files_list(void);

//// TEST
//// load the first file found as a blackfin ELF
extern void load_bfin_sdcard_test(void);


// load a specific numbered .ldr file
extern void load_bfin(u32 idx);
// load a specific named .ldr file
extern void load_bfin_name(const char* name);

// look for scenes dir, create if it doesn't exist
extern void files_check_scenes(void);




#endif // header guard 
