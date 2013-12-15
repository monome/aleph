// std
#include <string.h>

// asf
#include "flashc.h"
#include "print_funcs.h"

// aleph-avr32
#include "memory.h"
#include "flash.h"

// bees
#include "files.h"
#include "flash_bees.h"
#include "param_scaler.h"
#include "scene.h"

// buffer for scaler data in flash init
/// FIXME: this would be a temp buffer if we had malloc.
static s32* scalerBuf;
// max size of data in single scaler file
static const u32 scalerMaxValues = 1024;


typedef struct {
  u8 sceneBytes[sizeof(sceneData_t)];
  u8 scalerBytes[PARAM_SCALER_DATA_SIZE];
} beesFlashData;

// read default scene data to global buffer
void flash_read_scene(void) { 
  memcpy((void*)sceneData, flash_app_data(), sizeof(sceneData_t)); 
  scene_read_buf();
}

// write default scene data from global buffer
void flash_write_scene(void) { 
  scene_write_buf();
  print_dbg("\r\n writing scene data tto address: 0x");
  
  flashc_memcpy( &( ((beesFlashData*)(flash_app_data()))->sceneBytes),
		 (void*)sceneData, sizeof(sceneData_t), true);
}

// initialize nonvolatile scaler data
void flash_init_scaler_data(void) {
  ///// for each param type, get number of offline data bytes,
  ///// load file and write to flash if necessary
  u32 b;
  const char* path;
  u8 p;
  for(p=0; p<eParamNumTypes; ++p) {
    b = scaler_get_data_bytes(p);
    if(b > 0) {
      path = scaler_get_data_path(p);
      files_load_scaler_name(path, scalerBuf, scalerMaxValues);
      flashc_memcpy( (void*)(&( ((beesFlashData*)(flash_app_data()))->scalerBytes) + scaler_get_data_offset(p)),
		     (void*)sceneData, sizeof(sceneData_t), true);
      //      flash_write_scaler(scaler_get_data_offset(p));
    }
    b = scaler_get_rep_bytes(p);
    if(b > 0) {
      path = scaler_get_rep_path(p);
      files_load_scaler_name(path, scalerBuf, scalerMaxValues);
      flashc_memcpy( (void*)(&( ((beesFlashData*)(flash_app_data()))->scalerBytes) + scaler_get_rep_offset(p)),
		     (void*)sceneData, sizeof(sceneData_t), true);
      //      flash_write_scaler(scaler_get_rep_offset(p));
      ///      flashc_memcpy(flash_app_data(), scaler_get_rep_offset(p));
    }
  }
}

// initialize buffer
void flash_bees_init(void) {
  scalerBuf = (s32*)alloc_mem(scalerMaxValues * 4);
}

// write scaler data 
