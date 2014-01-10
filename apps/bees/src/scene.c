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
#include "pages.h"
#include "net_protected.h"
#include "preset.h"
#include "render.h"
#include "scene.h"
#include "types.h"

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
}

// fill global RAM buffer with current state of system
void scene_write_buf(void) {
  u8* dst = (u8*)(sceneData->pickle);

  u32 bytes = 0;
  u8* newDst = NULL;

  ///// print parmameters
  //  u32 i;

  print_dbg("\r\n writing scene data... ");

  /*  for(i=0; i<net->numParams; i++) {
    print_dbg("\r\n param ");
    print_dbg_ulong(i);
    print_dbg(" : ");
    print_dbg(net->params[i].desc.label);
    print_dbg(" ; val ");
    print_dbg_hex((u32)net->params[i].data.value.asInt);
  }
  */

  // pickle network
  newDst = net_pickle(dst);
  bytes += (newDst - dst);
  print_dbg("\r\n bytes written: 0x");
  print_dbg_hex(bytes);
  dst = newDst;
  // pickle presets
  newDst = presets_pickle(dst);
  bytes += (newDst - dst);
  print_dbg("\r\n bytes written: 0x");
  print_dbg_hex(bytes);
  dst = newDst;
  
  /* if((u32)dst > ((u32)(sceneData->pickle) + SCENE_PICKLE_SIZE)) { */
  /*   print_dbg(" !!!!!!!! error: serialized scene data is exceeded allocated bounds !!!!! "); */
  /* } */
  if(bytes > SCENE_PICKLE_SIZE) {
    print_dbg(" !!!!!!!! error: serialized scene data is exceeded allocated bounds !!!!! ");
  }
}

// set current state of system from global RAM buffer
void scene_read_buf(void) {
  s8 modName[MODULE_NAME_LEN];
  const u8* src = (u8*)&(sceneData->pickle);

  /// FIXME: we really should be using this comparison
  
  //  s8 neq = 0;
  //  u32 i;

  app_pause();

  // store current mod name in scene desc
  memcpy(modName, sceneData->desc.moduleName, MODULE_NAME_LEN);

  ///// always load:
    print_dbg("\r\n loading module name: ");
    print_dbg(sceneData->desc.moduleName);
    files_load_dsp_name(sceneData->desc.moduleName);
    //  }

    bfin_wait_ready();

    net_report_params();

  // unpickle network 
  print_dbg("\r\n unpickling network for scene recall...");
  src = net_unpickle(src);

  // unpickle presets
  print_dbg("\r\n unpickling presets for scene recall...");
  src = presets_unpickle(src);
  
  print_dbg("\r\n copied stored network and presets to RAM ");

  /* for(i=0; i<net->numParams; i++) { */
  /*   print_dbg("\r\n param "); */
  /*   print_dbg_ulong(i); */
  /*   print_dbg(" : "); */
  /*   print_dbg(net->params[i].desc.label); */
  /*   print_dbg(" ; val "); */
  /*   print_dbg_hex((u32)net->params[i].data.value); */
  /* } */

  // compare module name  

  //// is strncmp fucking with us??
  ///  neq = strncmp((const char*)modName, (const char*)sceneData->desc.moduleName, MODULE_NAME_LEN);
  
  //  if(neq) {
    // load bfin module if it doesn't match the current scene desc

    
    bfin_wait_ready();

  //// well let's try it, actually that would explain some things..
    //  delay_ms(10);

  // update bfin parameters
  net_send_params();
  print_dbg("\r\n sent new params");

  delay_ms(5);

  // enable audio processing
  bfin_enable();
  
  app_resume();
}

// write current state as default
void scene_write_default(void) {
  s8 neq = 0;
  s8 modName[MODULE_NAME_LEN];

  app_pause();
  render_boot("writing scene to flash");

  print_dbg("\r\n writing scene to flash... ");
  print_dbg("module name: ");
  print_dbg(sceneData->desc.moduleName);

  flash_write_scene();

  // write default LDR if changed 
  neq = strncmp((const char*)modName, (const char*)sceneData->desc.moduleName, MODULE_NAME_LEN);
  if(neq) {
    render_boot("writing DSP to flash");
    print_dbg("\r\n writing default LDR from scene descriptor");
    files_store_default_dsp_name(sceneData->desc.moduleName);
  }    
  delay_ms(20);
  print_dbg("\r\n finished writing default scene");
  app_resume();
}

// load from default
void scene_read_default(void) {
  app_pause();
  print_dbg("\r\n reading default scene from flash... ");
  flash_read_scene();
  
  print_dbg("\r\n finished reading ");  
  app_resume();
}

// set scene name
void scene_set_name(const char* name) {
  strncpy(sceneData->desc.sceneName, name, SCENE_NAME_LEN);
}

// set module name
void scene_set_module_name(const char* name) {
  strncpy(sceneData->desc.moduleName, name, MODULE_NAME_LEN);
}
