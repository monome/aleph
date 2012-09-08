#ifndef _FILES_H_
#define _FILES_H_

#include "compiler.h"

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
// load a blackfin executable from the "dsp" directory
extern void files_load_dsp(const char* name);

#endif // header guard 
