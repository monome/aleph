// std
#include <string.h>

// asf
#include "flashc.h"
#include "print_funcs.h"

// aleph-avr32
#include "flash.h"
#include "flash_bees.h"
#include "param_scaler.h"
#include "scene.h"


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
  
  flashc_memcpy( flash_app_data(), (void*)sceneData, sizeof(sceneData_t), true);
}
