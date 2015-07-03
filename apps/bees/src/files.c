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
#define DIR_LIST_PATH_LEN 64
#define DIR_LIST_EXT_LEN 8

#define DSP_PATH     "/mod/"
#define SCENES_PATH  "/data/bees/scenes/"
#define SCALERS_PATH  "/data/bees/scalers/"

// endinanness
// #define SCALER_LE 1

// stupid datatype with fixed number of fixed-length filenames
// storing this for speed when UI asks us for a lot of strings
typedef struct _dirList {
  char path[DIR_LIST_PATH_LEN];
  char ext[DIR_LIST_EXT_LEN];
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
static void list_fill(dirList_t* list, const char* path, const char* ext);
// get name at idx
static const char* list_get_name(dirList_t* list, u8 idx);
// get read file pointer if found (caller must close)
// set size by pointer
static void* list_open_file_name(dirList_t* list, const char* name, const char* mode, u32* size);


// fake fread: no size arg, always byte
static void fake_fread(volatile u8* dst, u32 len, void* fp) {
  u32 n = 0;
#if 0
  // this seems to fail with files over a certain size, or something.
  // weird because fl_fgetc is just a call to fl_fread with size=1, data=1.
  fl_fread(&dst, 1, len, fp);
#else
  while(n < len) {
    *dst = fl_fgetc(fp);
    n++;
    dst++;
  }
#endif
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

//---------------------------
//------------- extern defs

// check for externsion
bool check_ext(char* str, const char* extB ) {
  int i;
  int dotpos = -1;
  char* extA = NULL;
  bool res;
 
  i = strlen(str);
  while(i > 0) {
    --i;
    if(str[i] == '.') {
      dotpos = i;
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
  list_fill(&dspList, DSP_PATH, ".ldr");
  list_fill(&sceneList, SCENES_PATH, ".scn");
  list_fill(&scalerList, SCALERS_PATH, ".dat");
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
  //  char nameTry[DIR_LIST_NAME_LEN];
  volatile u8* bfinLdrData = NULL;

  delay_ms(10);
  app_pause();

  fp = list_open_file_name(&dspList, name, "r", &size);

  if (fp != NULL) {

    if(size > 0) {

      // allocate a RAM buffer
      bfinLdrData = alloc_mem(size);
      fake_fread(bfinLdrData, size, fp);
      fl_fclose(fp);

      // reboot the dsp with new firmware in RAM
      bfin_load_buf((const u8*)bfinLdrData, size);
      // free the buffer!
      free_mem(bfinLdrData);

      // write module name in global scene data

      /////////////////
      /// FIXME: filename and reported modulename should be decoupled
      /// bees should search for aleph-module-x.y.z.ldr
      /// but try aleph-module*.ldr on failure, etc
      ////
      /// query name and version to the scene data
      //      scene_query_module();
      scene_set_module_name(name);
      ///////////////////////////

      ret = files_load_desc(name);
      //???
      ret = 1;
    } else {
      ret = 0;
    }
  } else {
    ret = 0;
  }

  app_resume();
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
  void* fp;
  u32 size = 0;
  u8 ret = 0;

  // store the old module name
  char oldModuleName[MODULE_NAME_LEN];
  strncpy(oldModuleName, sceneData->desc.moduleName, MODULE_NAME_LEN);

  app_pause();

  fp = list_open_file_name(&sceneList, name, "r", &size);

  if( fp != NULL) {	  
    fake_fread((volatile u8*)sceneData, sizeof(sceneData_t), fp);
    /// copy old name back to descriptor field... dirty.
    strncpy(sceneData->desc.moduleName, oldModuleName, MODULE_NAME_LEN);
    fl_fclose(fp);
    // unpack buffer, rebuild network, reboot DSP
    scene_read_buf();
  } else {
    ret = 0;
  } 
  app_resume();
  return ret;
}


// store scene to sdcard at idx
void files_store_scene(u8 idx) {
  //  files_store_scene_name((const char*)files_get_scene_name(idx), 0);
  files_store_scene_name((const char*)files_get_scene_name(idx));
}

// store scene to sdcard at name
void files_store_scene_name(const char* name/*, u8 ext*/) {
  //u32 i;
  void* fp;
  char namebuf[64] = SCENES_PATH;
  u8* pScene;

  app_pause();

  strcat(namebuf, name);
  strip_ext(namebuf);
  //  if(ext) {
    // weird..
    strip_space(namebuf, 32);
    strcat(namebuf, ".scn");
    //  }


  // fill the scene RAM buffer from current state of system
  scene_write_buf(); 

  // open FP for writing
  fp = fl_fopen(namebuf, "wb");

  pScene = (u8*)sceneData;


  fl_fwrite((const void*)pScene, sizeof(sceneData_t), 1, fp);
  fl_fclose(fp);


  // rescan
  list_fill(&sceneList, SCENES_PATH, ".scn");
  delay_ms(10);


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

  app_pause();
  fp = list_open_file_name(&scalerList, name, "r", &size);
  if( fp != NULL) {	  

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


    if(size > dstSize) {
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
    ret = 0;
  } 


  ///// TEST: verify
  /* for(i=0; i<size; i++) { */
  /* } */

  app_resume();
  return ret;
}


//---------------------
//------ static

const char* list_get_name(dirList_t* list, u8 idx) {
  return (const char*) ( list->nameBuf + (idx * DIR_LIST_NAME_LEN) );
}

void list_fill(dirList_t* list, const char* path, const char* ext) {
  FL_DIR dirstat; 
  struct fs_dir_ent dirent;
  int i;

  // clear out the buffers...
  for(i=0; i<DIR_LIST_NAME_BUF_SIZE; ++i) {
    list->nameBuf[i] = '\0';
  }

  list->num = 0;
  strcpy(list->path, path);
  strcpy(list->ext, ext);

  if( fl_opendir(path, &dirstat) ) {      
    while (fl_readdir(&dirstat, &dirent) == 0) {
      if( !(dirent.is_dir) ) {

	if(check_ext(dirent.filename, ext)) {
	  ////////////////////////////////////
	  /// strip the extension before storing
	  strip_ext(dirent.filename);
	  /////////////////////////
	  strncpy((char*)(list->nameBuf + (list->num * DIR_LIST_NAME_LEN)),
		  dirent.filename, DIR_LIST_NAME_LEN_1);
	  *(list->nameBuf + (list->num * DIR_LIST_NAME_LEN) + DIR_LIST_NAME_LEN_1) = '\0';
	  list->num += 1;
	}
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
  char nameTry[DIR_LIST_NAME_LEN];


  // strip any .extension and add our own
  strcpy(path, list->path);
  strncpy(nameTry, name, DIR_LIST_NAME_LEN);
  strip_ext(nameTry);
  strncat(nameTry, list->ext, DIR_LIST_NAME_LEN);

  if(fl_opendir(path, &dirstat)) {
    
    while (fl_readdir(&dirstat, &dirent) == 0) {

      if (strcmp(dirent.filename, nameTry) == 0) {
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

  print_dbg("\r\n files_load_desc(): name = ");
  print_dbg(name);

  strcat(path, name);
  strip_ext(path);
  strcat(path, ".dsc");

  print_dbg("\r\n ...path = ");
  print_dbg(path);


  fp = fl_fopen(path, "r");
  if(fp == NULL) {
    print_dbg("\r\n file_load_desc(): fl_fopen(...) => NULL");
    ret = 1;
  } else {

    // get number of parameters
    fake_fread(nbuf, 4, fp);
    unpickle_32(nbuf, (u32*)&nparams);

    print_dbg("\r\n file_load_desc(): nparams = 0x");
    print_dbg_hex(nparams);

    // on boot the bfin is put back into spi slave mode before the
    // module_init() is called. since net_add_param() below ultimately
    // gets the default values from the module via spi we wait for the
    // bfin to be ready thus ensuring that module_init() has actually
    // had time to sets the parameter defaults.
    print_dbg("\r\n file_load_desc(): waiting for bfin to be ready before quering parameters");
    bfin_wait_ready();

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
      ret = 1;
    }
  }
  fl_fclose(fp);
  app_resume();
  return ret;
}
