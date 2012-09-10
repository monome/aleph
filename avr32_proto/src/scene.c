/* scene.c 
   acr32
   aleph

   avr32-specific scene management module
   includes operator network, DSP patch
*/

// std
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// asf
#include "fs_com.h"
#include "file.h"
#include "navigation.h"
// aleph
#include "files.h"
#include "fix.h"
#include "memory.h"
#include "simple_string.h"
// bees
#include "net_protected.h"
#include "preset.h"
#include "types.h"
#include "scene.h"

typedef struct _scene {
  char sceneName[SCENE_NAME_LEN];
  char moduleName[MODULE_NAME_LEN];
} scene_t;

static FILE* pSceneFile;
static char sceneFilename[16];
// scene storage memory
static scene_t* sceneData;

void scene_init(void) {
  u8 i;
  //sceneData = (scene_t*)malloc(SCENE_COUNT * sizeof(scene_t));
  sceneData = (scene_t*)alloc_mem(SCENE_COUNT * sizeof(scene_t));
  for(i=0; i<SCENE_COUNT; i++) {
    str_copy("scene                   ", sceneData[i].sceneName, SCENE_NAME_LEN);
    itoa_whole(i, sceneData[i].sceneName + 6, 4);
    str_copy("no module               ", sceneData[i].moduleName, MODULE_NAME_LEN);
  }
}

void scene_deinit(void) {
  //  free(sceneData);
}

// store on filesystem
void scene_store(u32 idx) {
  
  // open file  
  nav_dir_root();
  nav_filelist_findname("scenes", false);
  nav_filelist_findname((FS_STRING)scene_filename(idx), true);  

  /*
  //  snprintf(sceneFilename, 32, "aleph_scene_%d", (int)idx);
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
  //
  //  memcpy(&(sceneData[idx].net), &net, sizeof(net));
  */
}

// recall
void scene_recall(u32 idx) {
  // open file  
  snprintf(sceneFilename, 32, "aleph_scene_%d", (int)idx);
  pSceneFile = fopen(sceneFilename, "rb"); 
  // load scene name
  fread(sceneData[idx].sceneName, 1, SCENE_NAME_LEN, pSceneFile);
  // load module name
  fread(sceneData[idx].moduleName, 1, MODULE_NAME_LEN, pSceneFile);
  // load network
  fread((u8*)(&net), 1, sizeof(ctlnet_t), pSceneFile);
  // loda  presets 
  fread((u8*)(&presets), 1, sizeof(preset_t) * NET_PRESETS_MAX, pSceneFile);
  // close file
  fclose(pSceneFile);
  //
  //  memcpy(&net, &(sceneData[idx].net), sizeof(net));
}

// get scene name
const char* scene_name(const s16 idx) {
  if (idx >=0 && idx < SCENE_COUNT) {
    return sceneData[idx].sceneName;
  } else { return 0; }
}


// get module name
const char* scene_module_name(const s16 idx) {
  if (idx >=0 && idx < SCENE_COUNT) {
    return sceneData[idx].moduleName;
  } else { return 0; }
}

const char* scene_filename(const s16 idx) {
  if(idx >=0 ) {
    //    sceneFilename = "aleph_scene_1234";
    str_copy("aleph_scene_1234", sceneFilename, 16);
    itoa_whole(idx, sceneFilename + 12, 4);
    return sceneFilename;
  } else {
    return 0;
  }
}
