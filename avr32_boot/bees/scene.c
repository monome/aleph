/* scene.c 
   bees
   aleph

   scene management module
   includes operator network, DSP patch
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"
#include "global.h"
#include "memory.h"
#include "net_protected.h"
#include "preset.h"
#include "types.h"
#include "scene.h"

#if ARCH_AVR32
#else
static FILE* pSceneFile;
static char sceneFilename[32];
#endif 


/// 


//---------------------------
// static vars
// descriptors for all scenes on file
static sceneDesc_t* scenes;

//-----------------------------
// extern vars
// global pointer to current scene data
sceneData_t* gScene;

void scene_init(void) {
#if ARCH_AVR32



#else
  u8 i;
  sceneData = (sceneData_t*)alloc_mem(SCENE_COUNT * sizeof(scene_t));
    
  for(i=0; i<SCENE_COUNT; i++) {
    snprintf(sceneData[i].sceneName, SCENE_NAME_LEN, "scene_%d", i);
    snprintf(sceneData[i].moduleName, MODULE_NAME_LEN, "none");
  }
#endif
}

void scene_deinit(void) {
  //  free(sceneData);
}

scene_store_flash(u32 idx) {
  
}

// store 
void scene_store_file(u32 idx) {
#if ARCH_AVR32


#else
  // open file  
  snprintf(sceneFilename, 32, "aleph_scene_%d", (int)idx);
  pSceneFile = fopen(sceneFilename, "wb");
  // store scene name
  fwrite(sceneData[idx].sceneName, 1, SCENE_NAME_LEN, pSceneFile);
  // store module name
  fwrite(sceneData[idx].moduleName, 1, MODULE_NAME_LEN, pSceneFile);
  // store network
  fwrite((u8*)(&net), 1, sizeof(ctlnet_t), pSceneFile);
  // store presets 
  fwrite((u8*)(&presets), 1, sizeof(preset_t) * NET_PRESETS_MAX, pSceneFile);
  // close file
  fclose(pSceneFile);
#endif
}

// recall
void scene_recall_file(u32 idx) {
#if ARCH_AVR32

#else
  // open file  
  snprintf(sceneFilename, 32, "aleph_scene_%d", (int)idx);
  pSceneFile = fopen(sceneFilename, "rb"); 
  // load scene name
  fread(sceneData[idx].sceneName, 1, SCENE_NAME_LEN, pSceneFile);
  // load module name
  fread(sceneData[idx].moduleName, 1, MODULE_NAME_LEN, pSceneFile);
  // load network
  fread((u8*)(&net), 1, sizeof(ctlnet_t), pSceneFile);
  // load  presets 
  fread((u8*)(&presets), 1, sizeof(preset_t) * NET_PRESETS_MAX, pSceneFile);
  // close file
  fclose(pSceneFile);
  //
  //  memcpy(&net, &(sceneData[idx].net), sizeof(net));
#endif
}

// get scene name
const char* scene_name(const s16 idx) {
#if ARCH_AVR32
  return " dummmmmmmmm ";
#else
  if (idx >=0 && idx < SCENE_COUNT) {
    return sceneData[idx].sceneName;
  } else { return 0; }
#endif
}


// get module name
const char* scene_module_name(const s16 idx) {
#if ARCH_AVR32


#else
  if (idx >=0 && idx < SCENE_COUNT) {
    return sceneData[idx].moduleName;
  } else { return 0; }
#endif
}
