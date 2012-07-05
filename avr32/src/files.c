/* files.c
   aleph-avr32
  
   filesystem routines
*/

// asf
#include <stdlib.h>
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
//// load the first file found as a blackfin ELF
void load_bfin_elf(void) {
  U32 size;
  // sort by folders, then by files
  if (!nav_filelist_first(FS_DIR)) {
    nav_filelist_first(FS_FILE);
  }
  // take the first file
  nav_filelist_reset();
  nav_filelist_set( 0 , FS_FIND_NEXT);
  // get name
  nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
  // get size
  size = nav_file_lgt();
  if (size > 0) {
    load_buf = (char*)malloc( size );
  }
  ///////
  bfin_load(size, load_buf);
  ////////
  free(load_buf);
}

void files_list(void) {
  ///// LS  
  // Get the volume name
  nav_dir_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH);
  // Display general informations (drive letter and current path)
  print(DBG_USART, "\r\nVolume is ");
  print_char(DBG_USART, 'A' + nav_drive_get());
  print(DBG_USART, ":\r\nDir name is ");
  print(DBG_USART, name_buf);
  print(DBG_USART, CRLF);
  // Try to sort items by folders
  if (!nav_filelist_first(FS_DIR)) {
    // Sort items by files
    nav_filelist_first(FS_FILE);
  }
  // Display items informations
  print(DBG_USART, "\tSize (Bytes)\tName\r\n");
  // reset filelist before to start the listing
  nav_filelist_reset();
  // While an item can be found
  while (nav_filelist_set(0, FS_FIND_NEXT)) {
    // Get and display current item informations
    print(DBG_USART, (nav_file_isdir()) ? "Dir\t" : "   \t");
    print_ulong(DBG_USART, nav_file_lgt());
    print(DBG_USART, "\t\t");
    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    print(DBG_USART, name_buf);
    print(DBG_USART, CRLF);
    
    // Open the file.
    file_open(FOPEN_MODE_R);
    // While the end isn't reached
    while (!file_eof()) {
      // Display next char from file.
      print_char(DBG_USART, file_getc());
    }
    // Close the file.
    file_close();
    print(DBG_USART, CRLF);
    
  }
  // Display the files number
  print_ulong(DBG_USART, nav_filelist_nb(FS_FILE));
  print(DBG_USART, "  Files\r\n");
  // Display the folders number
  print_ulong(DBG_USART, nav_filelist_nb(FS_DIR));
  print(DBG_USART, "  Dir\r\n");
}
