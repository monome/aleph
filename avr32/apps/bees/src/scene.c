/* scene.c 
   bees
   aleph

   scene management module
   includes operator network, DSP patch
*/

// std
#include <string.h>
// asf
#include "delay.h"
//avr32
#include "app.h"
#include "bfin.h"
#include "font.h"
#include "print_funcs.h"
#include "simple_string.h"

/// bees
#include "files.h"
//#include "flash.h"
#include "flash_bees.h"
#include "global.h"
#include "memory.h"
#include "menu.h"
#include "net_protected.h"
#include "preset.h"
#include "types.h"
#include "scene.h"

//-----------------------------
// ---- extern data

// RAM buffer for scene data
sceneData_t* sceneData;

//----------------------------------------------
//----- extern functions

void scene_init(void) {
  u32 i;
  sceneData = (sceneData_t*)alloc_mem( sizeof(sceneData_t) );
  for(i=0; i<SCENE_NAME_LEN; i++) {
    (sceneData->desc.sceneName)[i] = ' ';
  }
  for(i=0; i<MODULE_NAME_LEN; i++) {
    (sceneData->desc.moduleName)[i] = ' ';
  }
}

void scene_deinit(void) {
  //  free(sceneData);
}

// fill global RAM buffer with current state of system
void scene_write_buf(void) {

  ///// print parmameters
  u32 i;
  print_dbg("\r\n writing scene data... ");
  for(i=0; i<net->numParams; i++) {
    print_dbg("\r\n param ");
    print_dbg_ulong(i);
    print_dbg(" : ");
    print_dbg(net->params[i].desc.label);
    print_dbg(" ; val ");
    print_dbg_hex((u32)net->params[i].data.value.asInt);
  }
  //////////////////////

  memcpy( &(sceneData->net),     (void*)net,  sizeof(ctlnet_t));
  print_dbg("\r\n copied network data to scene buffer.");
  memcpy( &(sceneData->presets), &presets, sizeof(preset_t) * NET_PRESETS_MAX); 
  print_dbg("\r\n copied preset data to scene buffer.");
}

// set current state of system from global RAM buffer
void scene_read_buf(void) {
  s8 modName[MODULE_NAME_LEN];
  u32 i;
  //  s8 neq = 0;

  app_pause();

  // store current mod name in scene desc
  memcpy(modName, sceneData->desc.moduleName, MODULE_NAME_LEN);

  // de-initialize network and operators
  net_deinit();

  // copy network/presets
  memcpy( (void*)net, &(sceneData->net),  sizeof(ctlnet_t) );
  memcpy( &presets, &(sceneData->presets), sizeof(preset_t) * NET_PRESETS_MAX );
  print_dbg("\r\n copied stored network and presets to RAM ");

  for(i=0; i<net->numParams; i++) {
    print_dbg("\r\n param ");
    print_dbg_ulong(i);
    print_dbg(" : ");
    print_dbg(net->params[i].desc.label);
    print_dbg(" ; val ");
    print_dbg_hex((u32)net->params[i].data.value.asInt);
  }

  // compare module name
  //  neq = strncmp((const char*)modName, (const char*)sceneData->desc.moduleName, MODULE_NAME_LEN);

  //  if(neq) {
  // load bfin module if it changed names

  print_dbg("\r\n loading module name: ");
  print_dbg(sceneData->desc.moduleName);
  files_load_dsp_name(sceneData->desc.moduleName);
  //app_pause();

  //  }

  //// TODO: module version check
  // "aaaabbbbccccddddeeeeffff"

  //  delay_ms(2000);
    
  // re-trigger inputs
  //  app_notify("re-initializing network/parameters");
  net_retrigger_inputs();
  
  // update bfin parameters
  net_send_params();
  print_dbg("\r\n sent new params");
  

  bfin_enable();
  app_resume();
}

// write current state as default
void scene_write_default(void) {
  app_pause();
  print_dbg("\r\n writing default scene to flash... ");
  print_dbg("module name: ");
  print_dbg(sceneData->desc.moduleName);
  scene_write_buf();
  flash_write_scene();
  print_dbg("\r\n finished writing ");
  app_resume();
}

// load from default
void scene_read_default(void) {
  app_pause();
  print_dbg("\r\n reading default scene from flash... ");
  flash_read_scene();
  scene_read_buf();
  print_dbg("\r\n finsihed reading ");  
  app_resume();
}

// set scene name
void scene_set_name(const char* name) {
  strncpy(sceneData->desc.sceneName, name, SCENE_NAME_LEN);
}

/* // set scene name char */
/* void scene_set_name_char(u8 idx, char ch) { */
/*   if(idx < SCENE_NAME_LEN) { */
/*     sceneData->desc.moduleName[idx] = ch; */
/*   } */
/* } */

// set module name
void scene_set_module_name(const char* name) {
  strncpy(sceneData->desc.moduleName, name, MODULE_NAME_LEN);
}
