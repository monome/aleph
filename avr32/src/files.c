/* files.c
   aleph-avr32
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
#include "compiler.h"
#include "print_funcs.h"

// aleph-avr32
#include "bfin.h"
#include "files.h"
#include "filesystem.h"


// list of names
//#define FILES_LIST_MAX_ENTRIES 128; 
#define MAX_NUM_DSP 32
#define DSP_NAME_LEN 32
#define DSP_LIST_BUF_SIZE 1024 // len * num

//------------------------------
//----- -static vars

volatile char dsp_name_buf[DSP_LIST_BUF_SIZE];
static u8 numDsp = 0;

//---------------------------------
//----- function definition

void init_files(void) {
  // init FAT lib
  fat_init();
  files_scan_dsp();

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
	print_dbg("\r\n adding dsp file : ");
	print_dbg(dirent.filename);
	strcpy((char *)dsp_name_buf + (numDsp * DSP_NAME_LEN), dirent.filename);
	numDsp++;
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
  //  unsigned long int i;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

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

	  //// we need to buffer the LDR file to save memory.
	  //// this sort of breaks modularity, oh well.
	  bfin_load(dirent.size, fp);
	  fl_fclose(fp);

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

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}

// return count of dsp files
u8 files_get_dsp_count(void) {
  return numDsp;
}
