/* scene.c 
   bees
   aleph

   scene management module
   includes operator network, DSP patch
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net_protected.h"
#include "preset.h"
#include "types.h"
#include "scene.h"

typedef struct _scene {
  ctlnet_t net;
  char sceneName[SCENE_NAME_LEN];
  char moduleName[MODULE_NAME_LEN];
  preset_t presets[NET_PRESETS_MAX];
} scene_t;

// scene storage memory
static scene_t* pSceneData;

void scene_init(void) {
  u8 i;
  pSceneData = (scene_t*)malloc(SCENE_COUNT * sizeof(scene_t));
  for(i=0; i<SCENE_COUNT; i++) {
    snprintf(pSceneData[i].sceneName, SCENE_NAME_LEN, " scene %d", i);
    snprintf(pSceneData[i].moduleName, MODULE_NAME_LEN, " no module");


  }
}

  void scene_deinit(void) {
  free(pSceneData);
}

// store 
void scene_store(u32 idx) {
  memcpy(&(pSceneData[idx].net), &net, sizeof(net));
}

// recall
void scene_recall(u32 idx) {
  memcpy(&net, &(pSceneData[idx].net), sizeof(net));
}

// get scene name
const char* scene_name(const s16 idx) {
  if (idx >=0 && idx < SCENE_COUNT) {
    return pSceneData[idx].sceneName;
  } else { return 0; }
}


// get module name
const char* scene_module_name(const s16 idx) {
  if (idx >=0 && idx < SCENE_COUNT) {
    return pSceneData[idx].moduleName;
  } else { return 0; }
}

