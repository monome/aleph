/* scene.c 
   bees
   aleph

   scene management module
   includes operator network, DSP patch
*/

#include <stdlib.h>

#include "net_protected.h"
#include "types.h"
#include "scene.h"

#define SCENE_COUNT 16

typedef struct _scene {
  ctlnet_t net;
  char moduleName[MODULE_NAME_LEN];
  preset_t presets[NET_PRESETS_MAX];
} scene_t;

// scene storage memory
static scene_t* pSceneData;

void scene_init(void) {
  pSceneData = (scene_t*)malloc(SCENE_COUNT * sizeof(scene_t));
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

