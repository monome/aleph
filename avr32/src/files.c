/* files.c
   aleph-avr32
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
#include "compiler.h"
//#include "navigation.h"
//#include "fat.h"
//#include "file.h"
#include "print_funcs.h"
//#include "sdramc.h"
// aleph
#include "bfin.h"
//#include "conf_aleph.h"
#include "files.h"
#include "filesystem.h"
#include "memory.h"


//// FIXME: using static allocation
#define FILES_STATIC_MEM 1


#if FILES_STATIC_MEM
// make as big as we can handle in static mem
#define MAX_BFIN_LDR_BYTES 6000
//#define MAX_BFIN_LDR_BYTES 22000
#else
// grab a RAM buffer big enough for bf533s instruction SRAM
#define MAX_BFIN_LDR_BYTES 64000
#endif 

// list of names
//#define FILES_LIST_MAX_ENTRIES 128; 
#define MAX_NUM_DSP 64
#define DSP_NAME_LEN 32
#define DSP_LIST_BUF_SIZE 2048 // len * num

//------------------------------
//----- -static vars


#if FILES_STATIC_MEM
// bfin ldr file buffer
volatile u8 load_buf[MAX_BFIN_LDR_BYTES];
// dsp name list  - global for speed
volatile char dsp_name_buf[DSP_LIST_BUF_SIZE];
#else
volatile u8* load_buf;
volatile char * dsp_name_buf;
#endif


// current path buffer
// current directory status
//static FL_DIR dirstat;
// current directory entry
//static struct fs_dir_ent dirent;
// count of dsp files
static u8 numDsp = 0;

//---------------------------------
//------ function declaration
//static int test_files(void);


//---------------------------------
//----- function definition

void init_files(void) {
  //heap_t tmp;
  // init FAT lib
  fat_init();

#if FILES_STATIC_MEM
#else
  // allocate RAM for blackfin boot image
  tmp = alloc_mem(MAX_BFIN_LDR_BYTES);
  if(tmp != ALLOC_FAIL) {
    load_buf = tmp;
  } else {
    print_dbg("\r\nallocation error in files init \r\n");
  }
  // allocate ram for dsp list
  tmp = (heap_t)alloc_mem(MAX_NUM_DSP * MAX_FILE_PATH_LENGTH);
  if(tmp != ALLOC_FAIL) {
    dsp_name_buf = (char*)tmp;
  } else {
    print_dbg("\r\n allocation error in files init \r\n");
  }
#endif
}

// populate the dsp filelist with names
// ( keeping the list is because the interface will ask us for strings a lot)
void files_scan_dsp(void) {
  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  numDsp = 0;
  if( fl_opendir("/dsp", &dirstat) ) {      
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if( !(dirent.is_dir) ) {
	numDsp++;
	strcpy((char *)dsp_name_buf + (numDsp * DSP_NAME_LEN), dirent.filename);
      }
    }
  }
}

// return filename for given index
const volatile char* files_get_dsp_name(u8 idx) {
  return dsp_name_buf + (idx * DSP_NAME_LEN);
}

// load a blacfkin executable by index */
void files_load_dsp(u8 idx) { 
  files_load_dsp_name((const char*)files_get_dsp_name(idx));
}

 // search for specified dsp file and load it
void files_load_dsp_name(const char* name) {
  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  char path[64];
  void* fp;
  unsigned long int i;

  if( fl_opendir("/dsp", &dirstat) ) {      
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if (strcmp(dirent.filename, name) == 0) {
	print_dbg("\r\n matched " );
	print_dbg(name);
	strcpy(path, "/dsp/");
	strncat(path, dirent.filename, 58);
	fp = fl_fopen(path, "r");
	if( fp != NULL) {	  
	  print_dbg("\r\n found file, loading dsp... \r\n");
	  if(dirent.size > MAX_BFIN_LDR_BYTES) {
	    print_dbg("\r\n .ldr file too big for bfin memory! \r\n");
	  } else {
	    // FIXME: wish this worked:
	    //	    fl_fread(load_buf, 1, dirent.size, fp);
	    for(i=0; i<dirent.size; i++) {
	      load_buf[i] = fl_fgetc(fp);
	    }
	    fl_fclose(fp);
	    bfin_load(dirent.size, load_buf);
	  }
	} else {
	  print_dbg("\r\n error: fp was null in load_dsp_name \r\n");
	}
	print_dbg("\r\n done loading bfin. \r\n");
	// processed a filename match, get out
	return;
      } else {
	print_dbg("\r\n tested " );
	print_dbg(dirent.filename);
	print_dbg(", no match ");
      }
    }
  } else {
    print_dbg("\r\n error opening dsp dir in load_dsp_name \r\n");
  }
}

// return count of dsp files
u8 files_get_dsp_count(void) {
  return numDsp;
}
