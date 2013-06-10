/* scene.c 
   bees
   aleph

   scene management module
   includes operator network, DSP patch
*/

// std
#include <string.h>

//avr32
#include "print_funcs.h"

/// bees
#include "files.h"
#include "flash.h"
#include "global.h"
#include "memory.h"
#include "menu.h"
#include "net_protected.h"
#include "preset.h"
#include "types.h"
#include "scene.h"

//----------------------------------------------
//----- extern variables
sceneData_t* sceneData;

//----------------------------------------------
//----- extern functions

void scene_init(void) {
  sceneData = (sceneData_t*)alloc_mem( sizeof(sceneData_t) );
}

void scene_deinit(void) {
  //  free(sceneData);
}

// fill global RAM buffer with current state of system
void scene_write_buf(void) {
  memcpy( &(sceneData->net),     (void*)net,  sizeof(ctlnet_t));
  memcpy( &(sceneData->presets), &presets, sizeof(preset_t) * NET_PRESETS_MAX);
}

// set current state of system from global RAM buffer
void scene_read_buf(void) {
  /////
  ////
  // FIX
  //  return;
  ////
  ///

  memcpy( (void*)net, &(sceneData->net),  sizeof(ctlnet_t) );
  memcpy( &presets, &(sceneData->presets), sizeof(preset_t) * NET_PRESETS_MAX );

  print_dbg("\r\n copied network and presets! sending params...");

  // update bfin parameters
  net_send_params();

  print_dbg("\r\n send params");
}

// write current state as default
void scene_write_default(void) {
  print_dbg("\r\n writing default scene to flash... ");
  scene_write_buf();
  flash_write_scene(sceneData);
  print_dbg("\r\n finished writing ");
}

// load from default
void scene_read_default(void) {
  print_dbg("\r\n reading default scene from flash... ");
  flash_read_scene(sceneData);
  scene_read_buf();
  print_dbg("\r\n finsihed reading ");  
}
