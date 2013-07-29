#ifndef _ALEPH_BEES_SCENE_H_
#define _ALEPH_BEES_SCENE_H_

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
  char sceneName[SCENE_NAME_LEN];
  char moduleName[MODULE_NAME_LEN];
} sceneDesc_t;

// scene storage layout type
typedef struct _sceneData {
  sceneDesc_t desc;
  ctlnet_t net;
  preset_t presets[NET_PRESETS_MAX];
} sceneData_t;

//----------------------------------------
//----- external variables
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

// set scene name
extern void scene_set_name(const char* name);
// set scene name, single character
extern void scene_set_name_char(u8 idx, char ch);
// set module name
extern void scene_set_module_name(const char* name);

/// convenience setters
// scroll character at given position in name
//extern u8 scene_inc_char(u8 idx, u8 pos);
//extern u8 scene_dec_char(u8 idx, u8 pos);
 
#endif
