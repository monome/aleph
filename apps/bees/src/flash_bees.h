#ifndef _ALEPH_AVR32_APPS_BEES_FLASH_H_
#define _ALEPH_AVR32_APPS_BEES_FLASH_H_

// initialize buffers
extern void flash_bees_init(void);

// initialize nonvolatile scaler data
extern void flash_init_scaler_data(void);

// read/write default scene values in flash
extern void flash_read_scene(void);
extern void flash_write_scene(void);
// read/write scaler data with arbitrary offset
extern void flash_read_scaler(u32 offset); 
extern void flash_write_scaler(u32 offset); 

#endif // h guard
