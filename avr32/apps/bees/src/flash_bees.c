#include <string.h>

#include "flash.h"
#include "flash_bees.h"
#include "scene.h"


// read default scene data to global buffer
void flash_read_scene(void) { 
  ///// FIXME: how to redefine flash data for different apps,,??? yikes
  //  memcpy((void*)sceneData, (void*)&(flash_nvram_data.sceneData), sizeof(sceneData_t)); 
  //  scene_read_buf();
}

// write default scene data from global buffer
void flash_write_scene(void) { 
  //  scene_write_buf();
  //  flashc_memcpy( (void*)(&(flash_nvram_data.sceneData)), (void*)sceneData, sizeof(sceneData_t), true);
}
