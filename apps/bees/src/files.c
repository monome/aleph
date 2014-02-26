/* files.c
   bees
   aleph
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
// asf
//#include "compiler.h"
#include "delay.h"
#include "print_funcs.h"

// aleph-common 
#include "module_common.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "filesystem.h"
#include "flash.h"
#include "memory.h"

/// bees
#include "files.h"
#include "net_protected.h"
#include "pages.h"
#include "param.h"
#include "scene.h"

// ---- directory list class
// params
#define DIR_LIST_MAX_NUM 64
#define DIR_LIST_NAME_LEN 64
#define DIR_LIST_NAME_LEN_1 63
#define DIR_LIST_NAME_BUF_SIZE 4096 // len * num

#define DSP_PATH     "/mod/"
#define SCENES_PATH  "/data/bees/scenes/"
#define SCALERS_PATH  "/data/bees/scalers/"

// endinanness
// #define SCALER_LE 1

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
static dirList_t scalerList;

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
/* // not used
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
    if(str[i] == '\0') { continue; }
    else if(str[i] == ' ') { str[i] = '\0'; }
    else { break; }
  }
}


// strip extension from the end of a string
static void strip_ext(char* str) {
  int i;
  int dotpos = -1;
  i = strlen(str);
  while(i > 0) {
    --i;
    if(str[i] == '.') {
      dotpos = i;
      break;
    }
  } 
  if(dotpos >= 0) {
    str[dotpos] = '\0';
  }
}


//---------------------------
//------------- extern defs

void files_init(void) {
  // scan directories
  print_dbg("\r\n BEES file_init, scanning directories..");
  list_scan(&dspList, DSP_PATH);
  list_scan(&sceneList, SCENES_PATH);
  list_scan(&scalerList, SCALERS_PATH);
}


////////////////////////
//// modules

// return filename for DSP given index in list
const volatile char* files_get_dsp_name(u8 idx) {
  return list_get_name(&dspList, idx);
}

// load a blacfkin executable by index */
u8 files_load_dsp(u8 idx) {  
  //  app_notify("loading dsp...");
  return files_load_dsp_name((const char*)files_get_dsp_name(idx));
}

// search for specified dsp file and load it
u8 files_load_dsp_name(const char* name) {
  void* fp;
  u32 size = 0;
  u8 ret;
  char nameTry[64];
  //  ModuleVersion modVers;

  delay_ms(10);

  app_pause();

  fp = list_open_file_name(&dspList, name, "r", &size);

  if(fp == NULL) {
    //// HACK
    // try adding ".ldr" because sometimes that happens... ugh
    strcpy(nameTry, name);
    strcat(nameTry, ".ldr");
    fp = list_open_file_name(&dspList, nameTry, "r", &size);
  }

  if( fp != NULL) {	  
    print_dbg("\r\n found file, loading dsp: ");
    print_dbg(name);
    fake_fread(bfinLdrData, size, fp);

    fl_fclose(fp);
    bfinLdrSize = size;

    if(bfinLdrSize > 0) {
      print_dbg("\r\n loading bfin from buf");
      // reboot the dsp with new firmware in RAM
      bfin_load_buf();
      print_dbg("\r\n finished load");
      // write module name in global scene data

      /////////////////
      /// FIXME: filename and reported modulename should be decoupled
      /// bees should search for aleph-module-x.y.z.ldr
      /// but try aleph-module*.ldr on failure
      ////
      /// query name and version to the scene data
      //      scene_query_module();
      /// now set it to the actual filename because we are dumb
      scene_set_module_name(name);
      ///////////////////////////

      print_dbg("\r\n sceneData->moduleName : ");
      print_dbg(name);
      
      print_dbg("\r\n loading parameter descriptor file...");
      ret = files_load_desc(name);

      ret = 1;
    } else {
      print_dbg("\r\n bfin ldr size was <=0, aborting");
      ret = 0;
    }
  } else {
    print_dbg("\r\n error: fp was null in files_load_dsp_name \r\n");
    ret = 0;
  }

  app_resume();
  return ret;
}


// store .ldr as default in internal flash, given index
#if 0
void files_store_default_dsp(u8 idx) {
  files_store_default_dsp_name((const char*)files_get_dsp_name(idx));
  /* const char* name; */
  /* void* fp;	   */
  /* u32 size; */

  /* app_pause(); */

  /* name = (const char*)files_get_dsp_name(idx); */
  /* fp = list_open_file_name(&dspList, name, "r", &size); */

  /* if( fp != NULL) { */
  /*   print_dbg("\r\n writing default DSP..."); */
  /*   bfinLdrSize = size; */
  /*   fl_fread((void*)bfinLdrData, 1, size, fp); */
  /*   flash_write_ldr(); */
  /*   fl_fclose(fp); */
  /*   print_dbg("finished writing LDR to flash"); */
    
  /* } else { */
  /*   print_dbg("\r\n error: fp was null in files_store_default_dsp \r\n"); */
  /* } */

  /* app_resume(); */
}
#endif

/*
// store .ldr as default in internal flash, given name
void files_store_default_dsp_name(const char* name) {
  //  const char* name;
  void* fp;	  
  u32 size;

  app_pause();

  //  name = (const char*)files_get_dsp_name(idx);
  fp = list_open_file_name(&dspList, name, "r", &size);

  if( fp != NULL) {
    print_dbg("\r\n writing default DSP...");
    bfinLdrSize = size;
    print_dbg(" , size: ");
    print_dbg_ulong(size);
    //    fl_fread((void*)bfinLdrData, 1, size, fp);
    fake_fread((void*)bfinLdrData, size, fp);

    // TEST: print module data
#if 0
    for(u32 i = 0; i<size; i += 4) {
 if((i % 16) == 0) {
	print_dbg("\r\n");
      }
      print_dbg(" 0x");
      print_dbg_hex(*((u32*)(bfinLdrData + i)));
    }
#endif

    flash_write_ldr();
    fl_fclose(fp);
    print_dbg("\r\n finished writing default LDR to flash");
    
  } else {
    print_dbg("\r\n error: fp was null in files_store_default_dsp \r\n");
  }

  app_resume();
}
*/


// return count of dsp files
u8 files_get_dsp_count(void) {
  return dspList.num;
}

////////////////////////
//// scenes

// return filename for scene given index in list
const volatile char* files_get_scene_name(u8 idx) {
  /// DEBUG
  /* char buf[SCENE_NAME_LEN]; */
  /* u8 j; */
  /* strncpy( buf, list_get_name(&sceneList, idx), SCENE_NAME_LEN ); */
  /* print_dbg("\r\n name: \r\n"); */
  /* print_dbg(buf); */
  /* print_dbg("\r\n name as byte array: \r\n"); */
  /* for(j=0; j<SCENE_NAME_LEN; j++) { */
  /*   print_dbg( " 0x"); */
  /*   print_dbg_hex(buf[j]); */
  /*   print_dbg( " " ); */
  /*   if(!(j%8)) { print_dbg("\r\n"); }  */
  /* } */
  /* print_dbg("\r\n"); */

  return list_get_name(&sceneList, idx);
}

// load scene by index */
u8 files_load_scene(u8 idx) {  
  return files_load_scene_name((const char*)files_get_scene_name(idx));
}

// search for specified scene file and load it
// return 1 on success, 0 on failure
u8 files_load_scene_name(const char* name) {
  void* fp;
  u32 size = 0;
  u8 ret = 0;

  app_pause();

  fp = list_open_file_name(&sceneList, name, "r", &size);

  if( fp != NULL) {	  
    fake_fread((volatile u8*)sceneData, sizeof(sceneData_t), fp);
    fl_fclose(fp);
    scene_read_buf();

    // try and load dsp module indicated by scene descriptor
    //// DUDE! NO!!! scene does this. when did this happen!
    //// probably snuck in in some merge.
    //    ret = files_load_dsp_name(sceneData->desc.moduleName);
  } else {
    print_dbg("\r\n error: fp was null in files_load_scene_name \r\n");
    ret = 0;
  } 

  app_resume();
  return ret;
}


// store scene to sdcard at idx
void files_store_scene(u8 idx) {
  files_store_scene_name((const char*)files_get_scene_name(idx), 0);
}


// store scene to sdcard at name
void files_store_scene_name(const char* name, u8 ext) {
  //u32 i;
  void* fp;
  char namebuf[64] = SCENES_PATH;
  u8* pScene;

  app_pause();

  strcat(namebuf, name);

  if(ext) {
    // weird..
    strip_space(namebuf, 32);
    strcat(namebuf, ".scn");
  }

  print_dbg("\r\n opening scene file for writing: ");
  print_dbg(namebuf);

  // fill the scene RAM buffer from current state of system
  scene_write_buf(); 
  print_dbg("\r\n filled scene binary buffer");

  // open FP for writing
  fp = fl_fopen(namebuf, "wb");
  print_dbg("\r\n opened file for binary write at 0x");
  print_dbg_hex((u32)fp);

  pScene = (u8*)sceneData;
  print_dbg("\r\n writing data from scene buffer at 0x");
  print_dbg_hex((u32)pScene);
  print_dbg(", size : ");
  print_dbg_hex(sizeof(sceneData_t));
  

  // dump the scene data to debug output...

  fl_fwrite((const void*)pScene, sizeof(sceneData_t), 1, fp);
  fl_fclose(fp);

  print_dbg("\r\n ... finished writing, closed file pointer");

  // rescan
  list_scan(&sceneList, SCENES_PATH);
  delay_ms(10);

  print_dbg("\r\n re-scanned scene file list and waited.");

  app_resume();
}


// return count of scene files
u8 files_get_scene_count(void) {
  return sceneList.num;
}

////////////////////////
//// scalers


// return filename for scaler given index in list
const volatile char* files_get_scaler_name(u8 idx) {
  return list_get_name(&scalerList, idx);
}

// search for specified scaler file and load it to specified buffer
// return 1 on success, 0 on failure
u8 files_load_scaler_name(const char* name, s32* dst, u32 dstSize) {
  void* fp;
  u32 size = 0;
  u32 i;
  union { u32 u; s32 s; u8 b[4]; } swap;
  u8 ret = 0;
  //// test
  //s32* p = dst;
  ///

  app_pause();
  fp = list_open_file_name(&scalerList, name, "r", &size);
  if( fp != NULL) {	  

    print_dbg("\r\n scaler file pointer: 0x");
    print_dbg_hex((u32)fp);
#ifdef SCALER_LE
    swap.b[3] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[0] = fl_fgetc(fp);
#else
    swap.b[0] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[3] = fl_fgetc(fp);
#endif
    size = swap.u;

    print_dbg("\r\n read size (words): 0x");
    print_dbg_ulong(size);

    if(size > dstSize) {
      print_dbg("\r\n warning: requested scaler data is > target, truncating");
      for(i=0; i<dstSize; ++i) {

#ifdef SCALER_LE
    swap.b[3] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[0] = fl_fgetc(fp);
#else
    swap.b[0] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[3] = fl_fgetc(fp);
#endif
	*dst++ = swap.s;
      }
    } else if (size < dstSize) {
      print_dbg("\r\n warning: requested scaler data is < target, padding");
      for(i=0; i<size; ++i) {
#ifdef SCALER_LE
    swap.b[3] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[0] = fl_fgetc(fp);
#else
    swap.b[0] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[3] = fl_fgetc(fp);
#endif
	*dst++ = swap.s;
      }
      // remainder
      size = dstSize - size;
      for(i=0; i<size; ++i) {
	*dst++ = 0;
      }
    } else {
      for(i=0; i<size; ++i) {
#ifdef SCALER_LE
    swap.b[3] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[0] = fl_fgetc(fp);
#else
    swap.b[0] = fl_fgetc(fp);
    swap.b[1] = fl_fgetc(fp);
    swap.b[2] = fl_fgetc(fp);
    swap.b[3] = fl_fgetc(fp);
#endif
	*dst++ = swap.s;
      }
    }
    fl_fclose(fp);
    ret = 1;
  } else {
    print_dbg("\r\n error: fp was null in files_load_scaler_name \r\n");
    ret = 0;
  } 

  print_dbg("\r\n finished loading scaler file (?)");

  ///// TEST: verify
  /* for(i=0; i<size; i++) { */
  /*   print_dbg(" 0x"); print_dbg_hex(p[i]); if((i%4)==0) { print_dbg("\r\n"); } */
  /* } */

  app_resume();
  return ret;
}


//---------------------
//------ static

const char* list_get_name(dirList_t* list, u8 idx) {
  return (const char*) ( list->nameBuf + (idx * DIR_LIST_NAME_LEN) );
}

void list_scan(dirList_t* list, const char* path) {
  FL_DIR dirstat; 
  struct fs_dir_ent dirent;
  int i;

  // clear out the buffers...
  for(i=0; i<DIR_LIST_NAME_BUF_SIZE; ++i) {
    list->nameBuf[i] = '\0';
  }

  list->num = 0;
  strcpy(list->path, path);

  if( fl_opendir(path, &dirstat) ) {      
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if( !(dirent.is_dir) ) {
	strncpy((char*)(list->nameBuf + (list->num * DIR_LIST_NAME_LEN)), dirent.filename, DIR_LIST_NAME_LEN_1);
	*(list->nameBuf + (list->num * DIR_LIST_NAME_LEN) + DIR_LIST_NAME_LEN_1) = '\0';
	print_dbg("\r\n added file: ");
	print_dbg(dirent.filename);
	print_dbg(" , count: ");
	print_dbg_ulong(list->num);
	list->num += 1;
      }
    }
  }
  print_dbg("\r\n scanned list at path: ");
  print_dbg(list->path);
  print_dbg(" , contents : \r\n");

  /* for(i=0; i<list->num; i++) { */
  /*   char buf[32]; */
  /*   u8 j; */
  /*   strncpy( buf, list_get_name(list, i), 32 ); */

  /*   print_dbg(buf); */

  /*   for(j=0; j<32; j++) { */
  /*     print_dbg( " 0x"); */
  /*     print_dbg_hex(buf[j]); */
  /*     print_dbg( " " ); */
  /*     if(!(j%8)) { print_dbg("\r\n"); } */
  /*   } */
  /*   print_dbg("\r\n"); */
  /* } */
  /* print_dbg("\r\n"); */
}

// search for a given filename in a listed directory. set size by pointer
void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size) {
  FL_DIR dirstat;
  struct fs_dir_ent dirent;
  char path[64];
  void* fp;

  print_dbg("\r\n *list_open_file_name: "); 
  print_dbg(path); 
  print_dbg(" at ");
  print_dbg(list->path);
  print_dbg(" request: ");
  print_dbg(name);


  strcpy(path, list->path);

  if(fl_opendir(path, &dirstat)) {
    
    while (fl_readdir(&dirstat, &dirent) == 0) {
      print_dbg("\r\n ... checking against "); 
      print_dbg(dirent.filename);

      if (strcmp(dirent.filename, name) == 0) {
	strncat(path, dirent.filename, 58);
	
	print_dbg("\r\n ... found, opening at:  "); 
	print_dbg(path);
      
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

// search for named .dsc file and load into network param desc memory
extern u8 files_load_desc(const char* name) {
  char path[64] = DSP_PATH;
  void * fp;
  int nparams = -1;
  // word buffer for 4-byte unpickling
  u8 nbuf[4];
  // buffer for binary blob of single descriptor
  u8 dbuf[PARAM_DESC_PICKLE_BYTES];
  // unpacked descriptor
  ParamDesc desc;
  int i;
  u8 ret = 0;

  app_pause();

  strcat(path, name);
  strip_ext(path);
  strcat(path, ".dsc");

  fp = fl_fopen(path, "r");
  if(fp == NULL) {
    print_dbg("\r\n error opening .dsc file, path: ");
    print_dbg(path);
    ret = 1;
  } else {

    // get number of parameters
    fake_fread(nbuf, 4, fp);
    unpickle_32(nbuf, (u32*)&nparams); 

    /// loop over params
    if(nparams > 0) {
      net_clear_params();
      //    net->numParams = nparams;

      for(i=0; i<nparams; i++) {
	//  FIXME: a little gross,
	// to be interleaving network and file manipulation like this...
	///....
	// read into desc buffer
	fake_fread(dbuf, PARAM_DESC_PICKLE_BYTES, fp);
	// unpickle directly into network descriptor memory
	pdesc_unpickle( &desc, dbuf );
	// copy descriptor to network and increment count
	net_add_param(i, (const ParamDesc*)(&desc));     
 
      }
    } else {
      print_dbg("\r\n error: crazy parameter count from descriptor file.");
      ret = 1;
    }
  }
  fl_fclose(fp);
  app_resume();
  return ret;
}



