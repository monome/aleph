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
#else
// grab an sdram buffer big enough for bf533s instruction SRAM
#define MAX_BFIN_LDR_BYTES 64000
#endif 

// list of names
//#define FILES_LIST_MAX_ENTRIES 128; 
#define MAX_FILE_PATH_LENGTH 128
#define MAX_NUM_DSP 64
#define DSP_NAME_LEN 32

//------------------------------
//----- -static vars

// buffer for filename

static char name_buf[MAX_FILE_PATH_LENGTH];
// memory for bfin load
#if FILES_STATIC_MEM
volatile u8 load_buf[MAX_BFIN_LDR_BYTES];
#else
volatile u8* load_buf;
#endif

// memory for list of dsp filenames
volatile char* dsp_name_buf;
// count of dsp files
static u8 numDsp = 0;

//---------------------------------
//------ function declaration
static int test_files(void);


//---------------------------------
//----- function definition
static int test_files(void) {  
  FL_DIR dirstat;
  struct fs_dir_ent dirent;

  void* fp;
  unsigned long int i;
  //  int data;
  
  char path[64];

  int res;
  int size;
 
  
  if( fl_opendir("/dsp", &dirstat) ) {
    print_dbg("\r\n opened directory /dsp/ \r\n");
    
    print_dbg("\r\n directory status pointer: ");
    print_dbg_hex((int)&dirstat);

    print_dbg("\r\n sector: ");
    print_dbg_hex(dirstat.sector);
    print_dbg("\r\n");

    print_dbg("\r\n cluster: ");
    print_dbg_hex(dirstat.cluster);
    print_dbg("\r\n");

    print_dbg("\r\n offset: ");
    print_dbg_hex(dirstat.offset);
    print_dbg("\r\n");
 
    res = fl_readdir(&dirstat, &dirent);
    print_dbg("\r\n readdir resut: ");
    print_dbg_hex(res);
    print_dbg("\r\n");
    
    while (fl_readdir(&dirstat, &dirent) == 0) {
      strcpy(path, "/dsp/");
      strncat(path, dirent.filename, 58);
      fp = fl_fopen(path, "r");
      if( fp != NULL) {
	print_dbg( "\r\nfile path: ");
	print_dbg( path);
	print_dbg( "\r\nfile pointer: ");
	print_dbg_hex( fp );
	print_dbg( "\r\nname: ");
	print_dbg( dirent.filename);
	print_dbg("\r\n");
	
	/* /\* for(i=0; i < dirent.size; i++) { *\/ */
	/* /\*   data =	fl_fgetc(fp);  /// or:	  //	fl_fread(&data, 1, 1, fp); *\/ */
	/* /\*   print_dbg_hex(data);	 *\/ */
	/* /\*   print_dbg(" "); *\/ */

	print_dbg("\r\n loading dsp... \r\n");
	if(dirent.size > MAX_BFIN_LDR_BYTES) {
	    print_dbg("\r\n .ldr file too big for bfin memory! \r\n");
	} else {
	  //	    fl_fread(load_buf, 1, dirent.size, fp);
	  for(i=0; i<dirent.size; i++) {
	    load_buf[i] = fl_fgetc(fp);

	    //// TEST
	    ///////
	    if(i < 1024) {
	      print_dbg(" ");
	      print_dbg_hex(load_buf[i]);
	      //      print_dbg(" ");
	    }
	    //////

	  }
	  fl_fclose(fp);
	  bfin_load(dirent.size, load_buf);
	}

	//		for(i=0; i < dirent.size; i++) { *\/ */
		    
		    //		    data = fl_fgetc(fp);  /// or:	  //	fl_fread(&data, 1, 1, fp); *\/ */
	/* /\*   print_dbg_hex(data);	 *\/ */
	/* /\*   print_dbg(" "); *\/ */	
	// } 
	//	print_dbg("\r\n");

      } else { 
	print_dbg("\r\n null file pointer in files test \r\n");
      }
    }
  } else {
    print_dbg("\r\n FAT test failed, couldn't open directory /dsp \r\n");
  }
  
}

void init_files(void) {
  heap_t tmp;
  // init FAT lib
  fat_init();

#if FILES_STATIC_MEM
#else
  // allocate SDRAM for blackfin boot image
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

  //  files_check_scenes();
  //  files_check_dsp();

  ///// TEST:
  test_files();
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
  //  nav_dir_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH);

  // Display general informations (drive letter and current path)
  //  print_dbg( "\r\nVolume is ");
  //  print_dbg_char( 'A' + nav_drive_get());
  //  print_dbg( ":\r\nDir name is ");
  //  print_dbg( name_buf);
  //  print_dbg( CRLF);

  // Try to sort items by folders
  // if (!nav_filelist_first(FS_DIR)) {
  // Sort items by files
  //    nav_filelist_first(FS_FILE);
  //  }

  // Display items informations
  //  print_dbg( "\tSize (Bytes)\tName\r\n");

  // reset filelist before to start the listing
  //  nav_filelist_reset();
  // While an item can be found
  while(0) {// while (nav_filelist_set(0, FS_FIND_NEXT)) {
    // Get and display current item informations
    //    print_dbg( (nav_file_isdir()) ? "Dir\t" : "   \t");
    //    print_dbg_ulong(nav_file_lgt());
    //    print_dbg( "\t\t");
    //    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    //    print_dbg( name_buf);
    //    print_dbg( CRLF);

  }
  // Display the files number
  //  print_dbg_ulong( nav_filelist_nb(FS_FILE));
  //  print_dbg( "  Files\r\n");
  // Display the folders number
  //  print_dbg_ulong( nav_filelist_nb(FS_DIR));
  //  print_dbg( "  Dir\r\n");
}

// look for scenes dir, create if it doesn't exist
void files_check_scenes(void) {
  ///  nav_dir_root();
  if(0) {//  if (nav_filelist_findname("scenes", 0)) {
    // ...
    //    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    //    print_dbg("\r\n found scenes dir, now at: ");
    //    print_dbg( name_buf);
    //    print_dbg( CRLF);
    //// print contents...
  } else {
    //    print_dbg("\r\n attempting to create scenes dir\n\r");
    //    nav_dir_make("scenes"); 
    // create empty scene fileS
    /// TODO
  }
}

// look for dsp dir, create if it doesn't exist
void files_check_dsp(void) {
  //  nav_dir_root();
  if(0) { //  if (nav_filelist_findname("dsp", 0)) {
    // ...
    //    nav_file_name((FS_STRING)name_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
    //    print_dbg("\r\n found dsp dir, now at: ");
    //    print_dbg( name_buf);
    //    print_dbg( CRLF);
    //// print contents...
  } else {
    //    print_dbg("\r\n attempting to create dsp dir");
    //    nav_dir_make("dsp"); 
  }
}

// populate the dsp filelist with names
void files_scan_dsp(void) {
  //u8 i = 0;
  //  nav_dir_root();
  if(0) {//  if (nav_filelist_findname("dsp", false)) {
    //    nav_filelist_first(FS_FILE);
    //    nav_filelist_reset();
    //    while (nav_filelist_set(0, FS_FIND_NEXT)) {
    //      nav_file_name( (FS_STRING)(dsp_name_buf + (i * MAX_FILE_PATH_LENGTH)), 
    //				MAX_FILE_PATH_LENGTH, 
    //				FS_NAME_GET, 
    //				true );
    //      i++;
    //    }
    //    numDsp = nav_filelist_nb(FS_FILE);
  } else {
    //    print_dbg("\r\ndsp scan failed (no dsp directory)\n\r");
    //    numDsp = 0;
  }

}

// return filename for given index
const volatile char* files_get_dsp_filename(u8 idx) {
  return 0;  //  return dsp_name_buf + (idx * MAX_FILE_PATH_LENGTH);
}

// load a blacfkin executable by index
void files_load_dsp(u8 idx) {
  //  files_load_dsp_name((const char*)files_get_dsp_filename(idx));
}

// load a blackfin executable by name
void files_load_dsp_name(const char* name) {
  //  u32 size;
  //  u32 byte = 0;
  //  nav_dir_root();
  if(0) {//  if (nav_filelist_findname("dsp", false)) {
    if(0) {//    if(nav_filelist_findname((FS_STRING)name, false)) {
      //      size = nav_file_lgt();
      if(0) {  //      if ( (size > 0) && (size < MAX_BFIN_LDR_BYTES) ) {
	//	file_open(FOPEN_MODE_R);

	/*
 ///////// TEST
 while(1) {
 file_close();
 file_open(FOPEN_MODE_R);
 print_dbg("\r\n\n\n");
 byte = 0;
 ///////////////
 while (!file_eof()) {
 load_buf[byte] = file_getc();
 byte++;
 }
 print_dbg("\r\bread bytes: ");
 print_dbg_hex(byte);
 print_dbg("\r\n");
	  
 //////////////
 for(byte=0; byte<64; byte++) {
 //	    print_dbg_hex(load_buf[byte]);
 //	    print_dbg(" ");
 }
 }
 /////////////
 */

	///	file_close();
	//	bfin_load(size, load_buf);
      } else {
	//	print_dbg("\r\ndsp load failed (too large): \n\r");
	//	print_dbg(name);
	//	print_dbg( CRLF);
      }
    } else {
      //      print_dbg("\r\ndsp load failed (no file): \n\r");
      //      print_dbg(name);
      //      print_dbg( CRLF);
    }   
  } else {
    //      print_dbg("\r\ndsp load failed (no dsp directory): \n\r");
    //      print_dbg(name);
    //      print_dbg( CRLF);
  }
}

// return count of dsp files
u8 files_get_dsp_count(void) {
  return numDsp;
}
