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
#define BFIN_LDR_MAX_BYTES 64000
// list of names
//#define FILES_LIST_MAX_ENTRIES 128; 

//------------------------------
//----- -static vars

// buffer for filename
static char name_buf[MAX_FILE_PATH_LENGTH];
// memory for bfin load
volatile u8*  load_buf;

//---------------------------------
//------ function declaration


//---------------------------------
//----- function definition

void init_files(void) {
  heap_t tmp;
  // Reset navigators .
  nav_reset();
  // Use the last drive available as default.
  nav_drive_set(nav_drive_nb() - 1);
  // Mount it.
  nav_partition_mount();
  // allocate SDRAM for blackfin boot image
  tmp = alloc_mem(BFIN_LDR_MAX_BYTES);
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


  /*
  print_dbg("\r\nallocated bfin load buffer at ");
  print_dbg_ulong((u32)load_buf);

  print_dbg("\r\nSDRAM starts at at ");
  print_dbg_ulong((u32)SDRAM);
  */

  files_check_scenes();
}
		    

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
  if ( (size > 0) && (size < BFIN_LDR_MAX_BYTES) ) {
    // allocate RAM buffer
    //    load_buf = (char*)malloc( size );

    // copy file to buf
    file_open(FOPEN_MODE_R);
    // While the end isn't reached
    while (!file_eof()) {
      load_buf[byte] = file_getc();
      byte++;
    }
    // Close the file.
    file_close();

    /*
    /// debug print first 1024 bytes
    print_dbg("\r\n");
    for(byte=0; byte < 1024; byte++) {
      print_dbg_ulong((u32)load_buf[byte]);
      print_dbg(" ");
    }
    */

    // load from the buf
    bfin_load(size, load_buf);
    // de-allocate the buf
    //free(load_buf);
  }
}

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

/*
volatile char* files_get_scene_file(u8 idx) {
  
}
*/
