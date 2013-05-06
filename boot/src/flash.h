/* flash.h
   aleph-avr32

   internal flash read/write

*/


#ifndef __ALEPH_FLASH_H__
#define __ALEPH_FLASH_H__

//#include "filesystem.h"
#include "scene.h"

// read/write default scene values in flash
extern void flash_read_scene(sceneData_t* sceneData, sceneDesc_t* sceneDesc);
extern void flash_write_scene(sceneData_t* sceneData, sceneDesc_t* sceneDesc);

// read/write default blackfin loader file
extern void flash_read_ldr_byte(u8* byte, u32 idx);
extern void flash_write_ldr_byte(u8* byte, u32 idx);

#endif // h guard
