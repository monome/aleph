#ifndef _FILES_H_
#define _FILES_H_

#include "compiler.h"

// initialize filesystem navigation
extern void init_files(void);
// list all files (LS)
extern void files_list(void);

//// TEST:
//// load the first file found as a blackfin ELF
extern void load_bfin_elf(void);
#endif // header guard 
