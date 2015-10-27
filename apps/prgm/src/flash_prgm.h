#ifndef _ALEPH_AVR32_APPS_BEES_FLASH_H_
#define _ALEPH_AVR32_APPS_BEES_FLASH_H_

#include "param_scaler.h"
#include "scene.h"


//  global data structure for current settings
extern prgmData_t *prgmData;

typedef struct {
    u8 prgmBytes[sizeof(prgmData_t)];
} prgmFlashData;


//  initialize buffers
extern void flash_prgm_init(void);

//  read/write current setting to flash
extern void flash_read_prgm(void);
extern void flash_write_prgm(void);

#endif // h guard
