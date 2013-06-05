#ifndef _ALEPH_BEES_SCENE_H_
#define _ALEPH_BEES_SCENE_H_

#include "module_common.h"
#include "net_protected.h"
#include "preset.h"
#include "types.h"

#define SCENE_COUNT 8
#define SCENE_NAME_LEN 24

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


// initialize memory
void scene_init(void);
// de-init
void scene_deinit(void);
// store to global buffer
void scene_store(u32 idx);

//void scene_store_file(u32 idx);
//void scene_store_flash(u32 idx);

// recall scene from the global buffer
void scene_recall(u32 idx);
//void scene_recall_file(u32 idx);
//void scene_recall_flash(u32 idx);

// clear
/* void scene_clear(u32 idx); */
/* // get scene name */
/* const char* scene_name(const s16 idx); */
/* // get module name */
/* const char* scene_module_name(const s16 idx); */
/* // get filename for idx */
/* const char* scene_filename(const s16 idx); */

#endif
