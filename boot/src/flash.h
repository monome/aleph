/* flash.h
   aleph-avr32

   internal flash read/write

*/


#ifndef __ALEPH_FLASH_H__
#define __ALEPH_FLASH_H__

//#include "filesystem.h"
#include "scene.h"

// intialize (check/write firstrun bytes)
// return 1 if this is the first run, 0 otherwise
extern u8 flash_init(void);

// read/write default scene values in flash
extern void flash_read_scene(sceneData_t* sceneData, sceneDesc_t* sceneDesc);
extern void flash_write_scene(sceneData_t* sceneData, sceneDesc_t* sceneDesc);

// read/write default blackfin loader file to/from global ram buffer 
extern void flash_read_ldr_data(void);
extern void flash_write_ldr_data(void);

#endif // h guard
