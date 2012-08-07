#ifndef _ALEPH_BEES_SCENE_H_
#define _ALEPH_BEES_SCENE_H_

#include "module_common.h"
#include "types.h"

#define SCENE_NAME_LEN 32
#define SCENE_COUNT 16

// initialize memory
void scene_init(void);
// de-init
void scene_deinit(void);
// store 
void scene_store(u32 idx);
// recall
void scene_recall(u32 idx);
// get scene name
const char* scene_name(const s16 idx);
// get module name
const char* scene_module_name(const s16 idx);

#endif
