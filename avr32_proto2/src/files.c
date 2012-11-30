/* files.c
   aleph-avr32
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
#include "compiler.h"
#include "navigation.h"
#include "fat.h"
#include "file.h"
#include "print_funcs.h"
#include "sdramc.h"
// aleph
#include "bfin.h"
#include "conf_aleph.h"
#include "files.h"
#include "memory.h"

// grab an sdram buffer big enough for bf533s instruction SRAM
#define MAX_BFIN_LDR_BYTES 64000
// list of names
//#define FILES_LIST_MAX_ENTRIES 128; 
#define MAX_NUM_DSP 64
#define DSP_NAME_LEN 32

//------------------------------
//----- -static vars

// buffer for filename
static char name_buf[MAX_FILE_PATH_LENGTH];
// memory for bfin load
volatile u8*  load_buf;
// memory for list of dsp filenames
volatile char* dsp_name_buf;
// count of dsp files
static u8 numDsp = 0;

//---------------------------------
//------ function declaration


//---------------------------------
//----- function definition

void init_files(void) {
  heap_t tmp;
  nav_reset();
  // use the last drive available
  nav_drive_set(nav_drive_nb() - 1);
  nav_partition_mount();
  // allocate SDRAM for blackfin boot image
  tmp = alloc_mem(MAX_BFIN_LDR_BYTES);
  if(tmp != ALLOC_FAIL) {
    load_buf = tmp;
  } else {
    print_dbg("\r\nallocation error in files init");
  }
  /*
  // allocate SDRAM for file list
  tmp = alloc_mem(FILES_LIST_MAX_ENTRIES);
  if(tmp != ALLOC_FAIL) {
    filelist = tmp;
  } else {
    print_dbg("\r\n\ allocation error in files init");
  }
  */
  
  // allocate SDRAM for dsp list
  tmp = (heap_t)alloc_mem(MAX_NUM_DSP * MAX_FILE_PATH_LENGTH);
  if(tmp != ALLOC_FAIL) {
    dsp_name_buf = (char*)tmp;
  } else {
    print_dbg("\r\n allocation error in files init");
  }


  files_check_scenes();
  files_check_dsp();
}
		    
/*
//// TEST:
//// load the first LDR file found
void load_bfin_sdcard_test(void) {
  U32 size;
  U32 byte = 0;
  U8 len;
  U8 found = 0;
  // sort by folders, then by files
  if (!nav_filelist_first(FS_DIR)) {
    nav_filelist_first(FS_FILE);
  }
  // take the first file
  nav_filelist_reset();
  // search for an .ldr extension
  while(found == 0) {
    nav_filelist_set( 0 , FS_FIND_NEXT);
    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    len = strlen(name_buf);
    if (
	(name_buf[len-4] == '.') &&
	(name_buf[len-3] == 'l') &&
	(name_buf[len-2] == 'd') &&
	(name_buf[len-1] == 'r') ) {
      found = 1;
    }
  }
  // get size
  size = nav_file_lgt();
  print_dbg( "\r\n found .ldr file: ");
  print_dbg( name_buf);
  print_dbg( "\r\n size: ");
  print_dbg_ulong( size);

  //  print_dbg( "\r\n loading...");
     // copy file to buf
    file_open(FOPEN_MODE_R);
    // While the end isn't reached
    while (!file_eof()) {
      load_buf[byte] = file_getc();
      byte++;
    }
    // Close the file.
    file_close();
    // load from the buf
    bfin_load(size, load_buf);
  }
}
*/

void files_list(void) {
  // Get the volume name
  nav_dir_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH);
  // Display general informations (drive letter and current path)
  print_dbg( "\r\nVolume is ");
  print_dbg_char( 'A' + nav_drive_get());
  print_dbg( ":\r\nDir name is ");
  print_dbg( name_buf);
  print_dbg( CRLF);
  // Try to sort items by folders
  if (!nav_filelist_first(FS_DIR)) {
    // Sort items by files
    nav_filelist_first(FS_FILE);
  }
  // Display items informations
  print_dbg( "\tSize (Bytes)\tName\r\n");
  // reset filelist before to start the listing
  nav_filelist_reset();
  // While an item can be found
  while (nav_filelist_set(0, FS_FIND_NEXT)) {
    // Get and display current item informations
    print_dbg( (nav_file_isdir()) ? "Dir\t" : "   \t");
    print_dbg_ulong(nav_file_lgt());
    print_dbg( "\t\t");
    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    print_dbg( name_buf);
    print_dbg( CRLF);

  }
  // Display the files number
  print_dbg_ulong( nav_filelist_nb(FS_FILE));
  print_dbg( "  Files\r\n");
  // Display the folders number
  print_dbg_ulong( nav_filelist_nb(FS_DIR));
  print_dbg( "  Dir\r\n");
}

// look for scenes dir, create if it doesn't exist
void files_check_scenes(void) {
  nav_dir_root();
  if (nav_filelist_findname("scenes", 0)) {
    // ...
    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    print_dbg("\r\n found scenes dir? now at: ");
    print_dbg( name_buf);
    print_dbg( CRLF);
    //// print contents...
  } else {
    print_dbg("\r\n attempting to create scenes dir");
    nav_dir_make("scenes"); 
    // create empty scene files
    
  }
}

// look for dsp dir, create if it doesn't exist
void files_check_dsp(void) {
  nav_dir_root();
  if (nav_filelist_findname("dsp", 0)) {
    // ...
    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    print_dbg("\r\n found dsp dir? now at: ");
    print_dbg( name_buf);
    print_dbg( CRLF);
    //// print contents...
  } else {
    print_dbg("\r\n attempting to create dsp dir");
    nav_dir_make("dsp"); 
  }
}

// populate the dsp filelist with names
void files_scan_dsp(void) {
  u8 i = 0;
  nav_dir_root();
  if (nav_filelist_findname("dsp", false)) {
    nav_filelist_first(FS_FILE);
    nav_filelist_reset();
    while (nav_filelist_set(0, FS_FIND_NEXT)) {
      nav_file_name( (FS_STRING)(dsp_name_buf + (i * MAX_FILE_PATH_LENGTH)), 
				MAX_FILE_PATH_LENGTH, 
				FS_NAME_GET, 
				true );
      i++;
    }
    numDsp = nav_filelist_nb(FS_FILE);
  } else {
    print_dbg("\r\ndsp scan failed (no dsp directory)");
    numDsp = 0;
  }

}

// return filename for given index
const volatile char* files_get_dsp_filename(u8 idx) {
  return dsp_name_buf + (idx * MAX_FILE_PATH_LENGTH);
}

// load a blacfkin executable by index
void files_load_dsp(u8 idx) {
  files_load_dsp_name((const char*)files_get_dsp_filename(idx));
}

// load a blackfin executable by name
void files_load_dsp_name(const char* name) {
  u32 size;
  u32 byte = 0;
  nav_dir_root();
  if (nav_filelist_findname("dsp", false)) {
    if(nav_filelist_findname((FS_STRING)name, false)) {
      size = nav_file_lgt();
      if ( (size > 0) && (size < MAX_BFIN_LDR_BYTES) ) {
	file_open(FOPEN_MODE_R);

	while (!file_eof()) {
	  load_buf[byte] = file_getc();
	  byte++;
	}
	file_close();
	bfin_load(size, load_buf);
      } else {
	print_dbg("\r\ndsp load failed (too large): ");
	print_dbg(name);
      }
    } else {
      print_dbg("\r\ndsp load failed (no file): ");
      print_dbg(name);
    }   
  } else {
      print_dbg("\r\ndsp load failed (no dsp directory): ");
      print_dbg(name);
  }
}

// return count of dsp files
u8 files_get_dsp_count(void) {
  return numDsp;
}
