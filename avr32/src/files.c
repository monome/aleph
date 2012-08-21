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
// aleph
#include "bfin.h"
#include "conf_aleph.h"
#include "files.h"

//------------------------------
//----- -static vars

// buffer for filename
static char name_buf[MAX_FILE_PATH_LENGTH];
// dynamically-allocated memory for bfin load
static char * load_buf;
// TEST: malloc fail? yep
//static char load_buf[1024];

//-------------------------
//----- function def

void init_files(void) {
  // Reset navigators .
  nav_reset();
  // Use the last drive available as default.
  nav_drive_set(nav_drive_nb() - 1);
  // Mount it.
  nav_partition_mount();
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
  if (size > 0) {
    // allocate RAM buffer
    load_buf = (char*)malloc( size );

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
    // de-allocate the buf
    free(load_buf);
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
