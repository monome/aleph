#ifndef _ALEPH_BEES_SCENE_H_
#define _ALEPH_BEES_SCENE_H_

#include "app_bees.h"
#include "net_protected.h"
#include "module_common.h"
#include "preset.h"
#include "types.h"

#define SCENE_DESC_COUNT 24
#define SCENE_NAME_LEN 24

//----------------------------------------
//----- types

// scene descriptor type
typedef struct _sceneDesc {
  // scene name
  char sceneName[SCENE_NAME_LEN];
  // lodaed module name
  char moduleName[MODULE_NAME_LEN];
  // module version 
  ModuleVersion moduleVersion;
  // bees version
  AppVersion beesVersion;
} sceneDesc_t;

/* switching to serialization. 
 i guess the proper way to do this would be
 to actually specify serialized size of each object.
 this seems like an incredible pain in the ass,
 so i am picking an arbitrary size and will be 
 vigilant w/r/t overreaching it...!!! oh man
*/
// this is only used for parsing application NVRAM space.
//#define SCENE_PICKLE_SIZE 0x19000
//#define SCENE_PICKLE_SIZE 0x22000

////
// FIXME:
/* we're not using NVRAM for scene right now.
   however, we still need to keep a statically allocated serial blob in RAM (dumb, i know...)
   we should be streaming the scene data to the sdcard with just a tiny RAM buffer.
*/
#define SCENE_PICKLE_SIZE 0x40000


typedef struct _sceneData {
  // txt descriptor
  sceneDesc_t desc;
  // a (huge) blob of serialized data (ugh)
  u8 pickle[SCENE_PICKLE_SIZE];
} sceneData_t;

//----------------------------------------
//----- external variables

// global data structure for current scene
extern sceneData_t* sceneData;

//----------------------------------------
//----- external functions

// initialize memory
extern void scene_init(void);
// de-init
extern void scene_deinit(void);

// fill global RAM buffer with current state of systemh
extern void scene_write_buf(void);
// set current state of system from global RAM buffer
extern void scene_read_buf(void);

// write current state as default
extern void scene_write_default(void);
// load from default
extern void scene_read_default(void);
// load "clean start" scene
extern void scene_read_clean(void);

// set scene name
extern void scene_set_name(const char* name);
// set scene name, single character
extern void scene_set_name_char(u8 idx, char ch);
// set module name
extern void scene_set_module_name(const char* name);


// query module name and version
extern void scene_query_module(void);

// get scene name
extern const char* scene_get_name(void);
// get module name
extern const char* scene_get_module_name(void);

 
#endif
