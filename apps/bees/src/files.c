/* files.c
   bees
   aleph
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
#include "compiler.h"
#include "delay.h"
#include "print_funcs.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
///////////////////////////////////
//// test
#include "events.h"
/////////////////////////////////
#include "filesystem.h"
#include "flash.h"
#include "memory.h"

/// bees
#include "files.h"
#include "scene.h"

// ---- directory list class
// params
#define DIR_LIST_MAX_NUM 64
#define DIR_LIST_NAME_LEN 32
#define DIR_LIST_NAME_BUF_SIZE 1024 // len * num

#define DSP_PATH     "/dsp/"
#define SCENES_PATH  "/bees/scenes/"

//  stupid datatype with fixed number of fixed-length filenames
// storing this for speed when UI asks us for a lot of strings
typedef struct _dirList {
  char path[64];
  volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
  u32 num;
} dirList_t;


//-----------------------------
//----  extern vars

//------------------------------
//----- -static vars

// directory lists;
static dirList_t dspList;
static dirList_t sceneList;

//----------------------------------
//---- static functions

// populate list with filenames and count
static void list_scan(dirList_t* list, const char* path);
// get name at idx
static const char* list_get_name(dirList_t* list, u8 idx);
// get read file pointer if found (caller must close)
// set size by pointer
static void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size);

//// FIXME: dumb and slow seek/read functions because the real ones are broken
//// fseek: no offset arg, assume its the first seek since file was opened 
/*
static void fake_fseek(void* fp, u32 loc) {
  u32 n = 0;
  u8 dum;
  while(n < loc) {
    dum = fl_fgetc(fp);
    n++;
  }
}
*/

// fread: no size arg
static void fake_fread(volatile u8* dst, u32 size, void* fp) {
  u32 n = 0;
  while(n < size) {
    *dst = fl_fgetc(fp);
    n++;
    dst++;
  }
}

// strip space from the end of a string
static void strip_space(char* str, u8 len) {
  u8 i;
  for( i=(len-1); i>0; i-- ) {
    if(str[i] == 0) { continue; }
    else if(str[i] == ' ') { str[i] = 0; }
    else { break; }
  }
}

//---------------------------
//------------- extern defs

void files_init(void) {
  // scan directories
  print_dbg("\r\n BEES file_init, scanning directories..");
  list_scan(&dspList, DSP_PATH);
  list_scan(&sceneList, SCENES_PATH);
}


//----- dsp management

// return filename for DSP given index in list
const volatile char* files_get_dsp_name(u8 idx) {
  return list_get_name(&dspList, idx);
}

// load a blacfkin executable by index */
void files_load_dsp(u8 idx) {  
  //  app_notify("loading dsp...");
  files_load_dsp_name((const char*)files_get_dsp_name(idx));
}

// search for specified dsp file and load it
void files_load_dsp_name(const char* name) {
  void* fp;
  u32 size = 0;

  delay_ms(10);

  app_pause();

  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {	  
    print_dbg("\r\n found file, loading dsp ");
    print_dbg(name);
    /// FIXME:
    /// arrg, why is fl_fread intermittently broken?
    /// check our media access functions against fat_filelib.c, i guess
    //    bytesRead = fl_fread((void*)bfinLdrData, 1, size, fp);

    print_dbg("\r\n bfinLdrData : @0x");
    print_dbg_hex( (u32)bfinLdrData );
    fake_fread(bfinLdrData, size, fp);

    // print_dbg("\r\n finished fakefread");
    fl_fclose(fp);
    bfinLdrSize = size;

    if(bfinLdrSize > 0) {
      print_dbg("\r\n loading bfin from buf");
      // reboot the dsp with new firmware in RAM
      bfin_load_buf();
      print_dbg("\r\n finished load");
      // write module name in global scene data
      scene_set_module_name(name);
    } else {
      print_dbg("\r\n bfin ldr size was <=0, aborting");
    }
  } else {
    print_dbg("\r\n error: fp was null in files_load_dsp_name \r\n");
  }

  app_resume();
}


// store .ldr as default in internal flash
void files_store_default_dsp(u8 idx) {
  const char* name;
  void* fp;	  
  u32 size;

  app_pause();

  name = (const char*)files_get_dsp_name(idx);
  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {
    print_dbg("\r\n writing default DSP...");
    bfinLdrSize = size;
    fl_fread((void*)bfinLdrData, 1, size, fp);
    flash_write_ldr();
    fl_fclose(fp);
    print_dbg("finished writing LDR to flash");
    
  } else {
    print_dbg("\r\n error: fp was null in files_store_default_dsp \r\n");
  }

  app_resume();
}

// return count of dsp files
u8 files_get_dsp_count(void) {
  return dspList.num;
}

//----- scenes management
// return filename for scene given index in list
const volatile char* files_get_scene_name(u8 idx) {
  return list_get_name(&sceneList, idx);
}

// load scene by index */
void files_load_scene(u8 idx) {  
  files_load_scene_name((const char*)files_get_scene_name(idx));
}

// search for specified scene file and load it
void files_load_scene_name(const char* name) {
  void* fp;
  u32 size = 0;

  app_pause();

  fp = list_open_file_name(&sceneList, name, "r", &size);

  if( fp != NULL) {	  
    fake_fread((volatile u8*)sceneData, sizeof(sceneData_t), fp);
    fl_fclose(fp);
    scene_read_buf();
  } else {
    print_dbg("\r\n error: fp was null in files_load_scene_name \r\n");
  }
 
  app_resume();
}


// store scene to sdcard at idx
void files_store_scene(u8 idx) {
  files_store_scene_name((const char*)files_get_scene_name(idx));
}


// store scene to sdcard at name
void files_store_scene_name(const char* name) {
  //u32 i;
  void* fp;
  char namebuf[64] = SCENES_PATH;
  u8* pScene;

  app_pause();

  strcat(namebuf, name);
  strip_space(namebuf, 32);
 
  print_dbg("\r\n write scene at: ");
  print_dbg(namebuf);  

  // fill the scene RAM buffer from current state of system
  scene_write_buf(); 

  // open FP for writing
  fp = fl_fopen(namebuf, "wb");

  pScene = (u8*)sceneData;

  fl_fwrite((const void*)pScene, sizeof(sceneData_t), 1, fp);
  list_scan(&sceneList, SCENES_PATH);
  fl_fclose(fp);

  delay_ms(10);
  app_resume();
}


// store scene from filesystem  as default in internal flash
//// NOTE: unimplemented and i'm not sure we really want to do this?
void files_store_default_scene(u8 idx) {
  /* const char* name; */
  /* void* fp;	   */
  /* u32 size; */

  /* app_pause(); */

  /* name = (const char*)files_get_scene_name(idx); */
  /* fp = list_open_file_name(&dspList, name, "r", &size); */

  /* if( fp != NULL) { */
  /*   print_dbg("\r\n warning: files_store_default_scene is unimplemented"); */
  /*   //// TODO */
  /*   fl_fclose(fp); */
  /*   //    print_dbg("finished storing default scene"); */
    
  /* } else { */
  /*   print_dbg("\r\n error: fp was null in files_store_default_scene \r\n"); */
  /* } */

  /* app_resume(); */
}

// return count of dsp files
u8 files_get_scene_count(void) {
  return sceneList.num;
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
	strcpy((char*)(list->nameBuf + (list->num * DIR_LIST_NAME_LEN)), dirent.filename);
	list->num++;
      }
    }
  }
  print_dbg("\r\n scanned list at path: ");
  print_dbg(list->path);
  print_dbg(" , contents : \r\n");
  print_dbg((const char*)list->nameBuf);
  print_dbg("\r\n");
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
      } else { // no match on this entry
	*size = 0;
	fp = NULL;
      }
    } // end loop
  } else { // dir error
    print_dbg("\r\n directory error.");
    *size = 0;
    fp = NULL;
  }
  return fp;
}

//////////
/// test
/* extern void files_load_test_scene(void) { */
/*   void* fp; */
/*   app_pause(); */
  
/*   fp = fl_fopen("/bees/scenes/test_default.scn", "r"); */
/*   print_dbg("\r\n opened test sceme. fp: 0x"); */
/*   print_dbg_hex((u32)fp); */

/*   if(fp == NULL) { */
/*     print_dbg("\r\n test scene file was NULL"); */
/*     app_resume(); */
/*     return; */
/*   } */
/*   fake_fread((volatile u8*)sceneData, sizeof(sceneData_t), fp); */
/*   scene_read_buf(); */


/*   fl_fclose(fp); */
/*   app_resume(); */
/* } */
