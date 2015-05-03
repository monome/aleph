/* files.c
   bees
   aleph
  
   filesystem routines
*/

// std
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// asf
//#include "compiler.h"
//#include "delay.h"
#include "print_funcs.h"

// aleph-common 
//#include "module_common.h"
// aleph-avr32
#include "app.h"

/// bees
#include "files.h"
#include "param.h"
#include "scene.h"

// beekeep
#include "ui_files.h"

// ---- directory list class
// params
#define DIR_LIST_MAX_NUM 64
#define DIR_LIST_NAME_LEN 32
#define DIR_LIST_NAME_BUF_SIZE 1024 // len * num

#define DSP_PATH     "/mod/"
#define SCENES_PATH  "/data/bees/scenes/"
#define SCALERS_PATH  "/data/bees/scalers/"

// endinanness
// #define SCALER_LE

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
/* static void fake_fread(volatile u8* dst, u32 size, void* fp) { */
/*   u32 n = 0; */
/*   while(n < size) { */
/*     *dst = fl_fgetc(fp); */
/*     n++; */
/*     dst++; */
/*   } */
/* } */

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

// check for externsion
bool check_ext(char* str, const char* extB ) {
  int i;
  //  int dotpos = -1;
  char* extA = NULL;
  bool res;
 
  i = strlen(str);
  while(i > 0) {
    --i;
    if(str[i] == '.') {
      //      dotpos = i;
      extA = str + i;
      break;
    }
  } 
  if(i < 0) { 
    // no extension
    return 0;
  } else {
    res = strcmp(extA, extB);
    if(res == 0) { return 1; } else { return 0; }
  }
}

// strip extension from the end of a string
// return 1 if found, 0 if not
bool strip_ext(char* str) {
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
    return 1;
  } else {
    return 0;
  }
}

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
  // don't need .ldr, but we do need .dsc...
  char descname[128];
  u8 nbuf[4];
  // buffer for binary blob of single descriptor
  u8 dbuf[PARAM_DESC_PICKLE_BYTES];
  // unpacked descriptor
  ParamDesc desc;
  u32 nparams;
  u8 ret = 0;
  FILE* fp;
  int i;
  strcpy(descname, workingDir);
  strcat(descname, name);
  strip_ext(descname);
  strcat(descname, ".dsc");

  fp = fopen(descname, "r");
  
  if(fp == NULL) {
    printf("\r\n module descriptor not found; path: %s", descname);
    ret = 1;
    return ret;
  }
  
  // get count of params
  fread(nbuf, 1, 4, fp);
  unpickle_32(nbuf, (u32*)&nparams); 

  /// loop over params
  if(nparams > 0) {
    printf("\r\n loading param descriptor; count: %d", nparams);
    net_clear_params();
    for(i=0; i<nparams; i++) {
      // read into desc buffer
      fread(dbuf, 1, PARAM_DESC_PICKLE_BYTES, fp);
      // unpickle directly into network descriptor memory
      pdesc_unpickle( &desc, dbuf );
      // copy descriptor to network and increment count
      net_add_param(i, (const ParamDesc*)(&desc));     
    }
  } else {
    ret = 1;
  }

  fclose(fp);

  scene_set_module_name(name);
  return ret;
}


// return count of dsp files
u8 files_get_dsp_count(void) {
  return dspList.num;
}

////////////////////////
//// scenes

// return filename for scene given index in list
const volatile char* files_get_scene_name(u8 idx) {
  return list_get_name(&sceneList, idx);
}

// load scene by index */
u8 files_load_scene(u8 idx) {  
  return files_load_scene_name((const char*)files_get_scene_name(idx));
}

// search for specified scene file and load it
// return 1 on success, 0 on failure
u8 files_load_scene_name(const char* name) {

  char path[64] = "";
  FILE* f;
  u8 ret = 1;
  
//  strcpy(path, workingDir);
  strcat(path, name);
  printf("\r\n attempting to open scene file; path: %s", path);
  
  
  f = fopen(path, "r");
  if(f == NULL) {
	  printf("\r\n couldn't find scene file; path: %s", path);
	  return 0;
  }
  fread(sceneData, sizeof(sceneData_t), 1, f);
  fclose(f);

  scene_read_buf();

  printf("\r\n loaded scene buffer, search DSP:");
  ret = files_load_dsp_name(sceneData->desc.moduleName);

  return ret;
}


// store scene to sdcard at idx
void files_store_scene(u8 idx) {
  files_store_scene_name((const char*)files_get_scene_name(idx));
}


// store scene to sdcard at name
void files_store_scene_name(const char* name) {
  FILE* f = fopen(name, "w");
  scene_write_buf();
  fwrite((const void*)sceneData, sizeof(sceneData_t), 1, f);
  fclose(f);
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
#if 1
  return 0;
#else
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
#endif
}


//---------------------
//------ static

const char* list_get_name(dirList_t* list, u8 idx) {
  return (const char*) ( list->nameBuf + (idx * DIR_LIST_NAME_LEN) );
}

void list_scan(dirList_t* list, const char* path) {
  //  FL_DIR dirstat; 
  /* struct fs_dir_ent dirent; */

  /* list->num = 0; */
  /* strcpy(list->path, path); */

  /* if( fl_opendir(path, &dirstat) ) {       */
  /*   while (fl_readdir(&dirstat, &dirent) == 0) { */
  /*     if( !(dirent.is_dir) ) { */
  /* 	strcpy((char*)(list->nameBuf + (list->num * DIR_LIST_NAME_LEN)), dirent.filename); */
  /* 	print_dbg("\r\n added file: "); */
  /* 	print_dbg(dirent.filename); */
  /* 	print_dbg(" , count: "); */
  /* 	print_dbg_ulong(list->num); */
  /* 	list->num += 1; */
  /*     } */
  /*   } */
  /* } */
  /* print_dbg("\r\n scanned list at path: "); */
  /* print_dbg(list->path); */
  /* print_dbg(" , contents : \r\n"); */
  /* print_dbg((const char*)list->nameBuf); */
  /* print_dbg("\r\n"); */
}

// search for a given filename in a listed directory. set size by pointer
void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size) {
  /* FL_DIR dirstat; */
  /* struct fs_dir_ent dirent; */
  /* char path[64]; */
  /* void* fp; */

  /* strcpy(path, list->path); */

  /* if(fl_opendir(path, &dirstat)) { */
    
  /*   while (fl_readdir(&dirstat, &dirent) == 0) { */
  /*     if (strcmp(dirent.filename, name) == 0) { */
  /* 	strncat(path, dirent.filename, 58); */
  /* 	fp = fl_fopen(path, mode); */
  /* 	*size = dirent.size; */
  /* 	break; */
  /*     } else { // no match on this entry */
  /* 	*size = 0; */
  /* 	fp = NULL; */
  /*     } */
  /*   } // end loop */
  /* } else { // dir error */
  /*   print_dbg("\r\n directory error."); */
  /*   *size = 0; */
  /*   fp = NULL; */
  /* } */
  /* return fp; */
  return NULL;
}
