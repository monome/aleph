 vc/* scene.c 
   avr32
   aleph

   avr32-specific scene management module
   includes operator network, DSP patch
*/


// std
#include "string.h"

// aleph
#include "files.h"
#include "fix.h"
#include "types.h"

// bees
#include "net_protected.h"
#include "preset.h"
#include "scene.h"

// avr32
#include "global.h"

// restore data from the global scene buffer
void scene_recall(void) {
  u16 i;
o  //  memcpy( &, &(((sceneData_t*)sceneData)->desc), sizeof(sceneDesc_t), 1);
  memcpy( &ctlnet, &(((sceneData_t*)sceneData)->net), sizeof(ctlnet_t), 1);
  for(i=0; i<NET_PRESETS_MAX; i++) {
    memcpy(&(presets[i]), &((((sceneData_t*)sceneData)->presets)[i]), sizeof(preset_t), 1);
  }
  // check .ldr filename (TODO: checksum) and load if necessary
  if(strcmp() == 0) {
  } else {
    // TODO notify an error
  }
}


void scene_init(void) {
  sceneData = alloc_mem(sizeof(sceneData_t));
}

void scene_deinit(void) {
  // ... 
}
sce 
// store current network/presets in global buffer
void scene_store(u32 idx) {
  u16 i;
  memcpy( &ctlnet, &(((sceneData_t*)sceneData)->net), sizeof(ctlnet_t), 1);
  for(i=0; i<NET_PRESETS_MAX; i++) {
    memcpy(&(presets[i]), &((((sceneData_t*)sceneData)->presets)[i]), sizeof(preset_t), 1);
  }
}
