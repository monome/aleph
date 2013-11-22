/* files.c
   aleph-avr32
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
#include "compiler.h"
#include "delay.h"

// aleph-avr32
#include "bfin.h"
#include "files.h"
#include "filesystem.h"
#include "flash.h"
#include "memory.h"
#include "screen.h"
#include "menu.h"
#include "watchdog.h"

// ---- directory list class
// params
#define DIR_LIST_MAX_NUM 64
#define DIR_LIST_NAME_LEN 64
#define DIR_LIST_NAME_BUF_SIZE 2048 // len * num

/// show the size (progress fills) 
static void show_size(u32 size) {
  u32 x = 0;
  u32 y = 0;
  u32 i = 0;
  for(i = 0; i< (size / 0x200); ++i) {
    screen_pixel(x, y, 0xf);
    screen_pixel(x, y+1, 0xf);
    screen_pixel(x+1, y, 0xf);
    screen_pixel(x+1, y+1, 0xf);
    ++x;
    ++x;
    if(x >= NCOLS) {
      x = 0;
      ++y;
      ++y;
    }
    if (y > NROWS) {
      y = 0;
    }
  }
  screen_refresh();
}

/// dumb progress display
static void show_progress(u8 val) {
  static u32 x = 0;
  static u32 y = 00;
  static const u8 level = 0x0;
  screen_pixel(x, y, val | level);
  screen_pixel(x, y+1, val | level);
  screen_pixel(x+1, y, val | level);
  screen_pixel(x+1, y+1, val | level);
  screen_refresh();
  ++x;
  ++x;
  if(x >= NCOLS) {
    x = 0;
    ++y;
    ++y;
  }
  if (y > NROWS) {
    y = 0;
  }
}

//  stupid datatype with fixed number of fixed-length filenames
// storing this for speed when UI asks us for a lot of strings
typedef struct _dirList {
  char path[8];
  volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
  u32 num;
} dirList_t;


//-----------------------------
//----  extern vars

//------------------------------
//----- -static vars

// directory liests
static dirList_t dspList;
static dirList_t appList;

/// data for a single hex record

//----------------------------------
//---- static functions

// populate list with filenames and count
static void list_scan(dirList_t* list, const char* path);
// get name at idx
const char* list_get_name(dirList_t* list, u8 idx);
// get file pointer if found (caller must close)
//static void* list_open_file_name(dirList_t * list, const char* name, const char* mode);
// ugly, set size by pointer
static void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size);

// fread: no size arg
static void fake_fread(volatile u8* dst, u32 size, void* fp) {
  u32 n = 0;
  while(n < size) {
    *dst = fl_fgetc(fp);
    n++;
    dst++;
  }
}

//---------------------------
//------------- extern defs

void init_files(void) {
  // init FAT lib
  fat_init();
  // scan directories
  list_scan(&dspList, "/mod/");
  list_scan(&appList, "/app/");
}

// return filename for DSP given index in list
const volatile char* files_get_dsp_name(u8 idx) {
  return list_get_name(&dspList, idx);
}

// return filename for firmware given index in list
const volatile char* files_get_firmware_name(u8 idx) {
  return list_get_name(&appList, idx);
}

// load a blacfkin executable by index */
void files_load_dsp(u8 idx) {  
  files_load_dsp_name((const char*)files_get_dsp_name(idx));
}

// search for specified dsp file and load it
void files_load_dsp_name(const char* name) {
  void* fp;
  u32 size = 0;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {	  
    fake_fread(bfinLdrData, size, fp);

    fl_fclose(fp);
    bfinLdrSize = size;
    bfin_load_buf();
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}


/* // store .ldr as default in internal flash */
/* void files_store_default_dsp(u8 idx) { */
/*   const char* name; */
/*   void* fp;	   */
/*   u32 size; */

/*   cpu_irq_disable_level(APP_TC_IRQ_PRIORITY); */
/*   cpu_irq_disable_level(UI_IRQ_PRIORITY); */

/*   name = (const char*)files_get_dsp_name(idx); */
/*   fp = list_open_file_name(&dspList, name, "r", &size); */

/*   if( fp != NULL) { */
/*     bfinLdrSize = size; */
/*     fl_fread((void*)bfinLdrData, 1, size, fp); */
/*     flash_write_ldr(); */
/*     fl_fclose(fp); */
/*   } */

/*   cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*   cpu_irq_enable_level(UI_IRQ_PRIORITY); */
/* } */

// return count of dsp files
u8 files_get_dsp_count(void) {
  return dspList.num;
}

// return count of bin files
u8 files_get_firmware_count(void) {
  return appList.num;
}

// update firmware from binary file idx
void files_write_firmware(u8 idx) {
  files_write_firmware_name((const char*)files_get_firmware_name(idx));
}

// update firmware from binary file name
void files_write_firmware_name(const char* name) {
  static u8 hexRecordData[256];   // hex record buffer
  int ch = 0; // character (or EOF) returned by fgetc
  void* fp;   // file pointer
  u32 size;   // file size
  u32 fIdx;   // byte index in file
  u8 hIdx = 0;   // byte index in hex record

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  fp = list_open_file_name(&appList, name, "r", &size);

  //  // print_dbg("\r\n writing firmware to flash... ");
  if( fp != NULL) {   

    screen_clear();
    show_size(size);


    /// using intel-hex format
    for(fIdx = 0; fIdx<size; fIdx++) {
      ch = fl_fgetc(fp);
      if(ch == ':') {
	// beginning of a record;
	// send the last record if there was one
	if(hIdx > 0) {
	  flash_write_hex_record(hexRecordData);
	}
	// reset hex byte index for next record
	hIdx = 0;
      }

      hexRecordData[hIdx] = (u8)ch;
      ++hIdx;

      ///// show progress
      if( (fIdx % 0x200) == 0) {
	show_progress(ch);
      }
    }

    /// raw binary (not good)
    /* while( */
    /* ch = fl_getc(fp); */
    /* if(ch == EOF) { */
    /* } else  */
    /////

    fl_fclose(fp);
    //    // print_dbg("finished writing.\r\n");
    //    // print_dbg("rebooting now.");

    // clear firstrun field
    flash_clear_firstrun();

    Disable_global_interrupt();

    watchdog_reset();
        
  } else {
    // print_dbg("\r\n error: fp was null in files_write_firmware_name\r\n");
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
}


//---------------------
//------ static

const char* list_get_name(dirList_t* list, u8 idx) {
  return (const char*) ( list->nameBuf + (idx * DIR_LIST_NAME_LEN) );
}

void list_scan(dirList_t* list, const char* path) {
  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  list->num = 0;
  strcpy(list->path, path);
  if( fl_opendir(path, &dirstat) ) {      
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if( !(dirent.is_dir) ) {
	strcpy((char*)list->nameBuf + (list->num * DIR_LIST_NAME_LEN), dirent.filename);
	list->num++;
      }
    }
  }
}

// search for a given filename in a listed directory. set size by pointer
void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size) {

  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  char path[64];
  void* fp;
  strcpy(path, list->path);

  if(fl_opendir(path, &dirstat)) {
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if (strcmp(dirent.filename, name) == 0) {
	strncat(path, dirent.filename, 58);

	fp = fl_fopen(path, mode);
	*size = dirent.size;
	break;
      } else { // no match
	*size = 0;
	fp = NULL;
      }
    } // end loop
  } else { // dir error
    *size = 0;
    fp = NULL;
  }
  return fp;
}
