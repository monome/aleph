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


// read default scene data to global buffer
void flash_read_scene(void) { 
  memcpy((void*)sceneData, flash_app_data(), sizeof(sceneData_t)); 
  scene_read_buf();
}

// write default scene data from global buffer
void flash_write_scene(void) { 
  scene_write_buf();
  //  print_dbg("\r\n writing scene data to address: 0x");
  
#ifdef SCENE_FLASH
  flashc_memcpy( &( ((beesFlashData*)(flash_app_data()))->sceneBytes),
		 (void*)sceneData, sizeof(sceneData_t), true);
#endif
}

// initialize nonvolatile scaler data
void flash_init_scaler_data(void) {
  ///// for each param type, get number of offline data bytes,
  ///// load file and write to flash if necessary
  u32 b;
  const char* path;
  u8 p;
  u8* dst;
  void* scalerBytes;

  print_dbg("\r\n initializing NV data for param scaling, total bytes: ");
  print_dbg_ulong(PARAM_SCALER_DATA_SIZE);

  print_dbg("\r\n application data address in flash: 0x");
  print_dbg_hex((u32)flash_app_data());
  print_dbg("\r\n scaler data address: 0x");
  print_dbg_hex((u32)  &(((beesFlashData*)(flash_app_data()))->scalerBytes) );

  scalerBytes = (void*)&(((beesFlashData*)(flash_app_data()))->scalerBytes);

  for(p=0; p<eParamNumTypes; ++p) {
    b = scaler_get_data_bytes(p);
    if(b > 0) {
      path = scaler_get_data_path(p);
      print_dbg("\r\n writing scaler data to flash: ");
      print_dbg(path);
      files_load_scaler_name(path, scalerBuf, scalerMaxValues);

      /// TEST: print values from file
      /* print_dbg("\r\n values read from file: "); */
      /* for(u32 i=0; i<(b>>2); ++i) { */
      /* 	print_dbg("\r\n 0x"); */
      /* 	print_dbg_hex(scalerBuf[i]); */
      /* } */
      /* print_dbg("\r\n first/last values read from file: "); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex(scalerBuf[0]); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex(scalerBuf[ (b >> 2) - 1]); */

      //      dst = (void*)(&( ((beesFlashData*)(flash_app_data()))->scalerBytes) + scaler_get_data_offset(p));
      //      dst = scalerBytes + scaler_get_data_offset(p);
      dst = (void*)scaler_get_nv_data(p);
      print_dbg("\r\n writing scaler val data to flash at address: 0x");
      print_dbg_hex((u32)dst);
      flashc_memcpy( dst, (void*)scalerBuf, b, true);

      /// TEST: print values from flash
      /* print_dbg("\r\n values in flash after write: "); */
      /* for(u32 i=0; i<(b>>2); ++i) { */
      /* 	s32* src = (s32*)(dst + (i << 2)); */
      /* 	print_dbg("\r\n 0x"); */
      /* 	print_dbg_hex((u32)(*src)); */
      /* } */
      /* print_dbg("\r\n first/last values in flash after write: "); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex((u32)(*((s32*)dst))); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex((u32)(*((s32*)(dst + b - 4)))); */

    }
    b = scaler_get_rep_bytes(p);
    if(b > 0) {
      path = scaler_get_rep_path(p);
      print_dbg("\r\n writing scaler data to flash: ");
      print_dbg(path);
      files_load_scaler_name(path, scalerBuf, scalerMaxValues);

      /// TEST: print values from file
      /* print_dbg("\r\n values read from file: "); */
      /* for(u32 i=0; i<(b>>2); ++i) { */
      /* 	print_dbg("\r\n 0x"); */
      /* 	print_dbg_hex(scalerBuf[i]); */
      /* } */
      /* print_dbg("\r\n first/last values read from file: "); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex(scalerBuf[0]); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex(scalerBuf[ (b >> 2) - 1]); */

      //      dst = (void*)(&( ((beesFlashData*)(flash_app_data()))->scalerBytes) + scaler_get_rep_offset(p));
      //      dst = scalerBytes + scaler_get_rep_offset(p);
      dst = (void*)scaler_get_nv_rep(p);
      print_dbg("\r\n writing scaler rep data to flash at address: 0x");
      print_dbg_hex((u32)dst);
      flashc_memcpy( dst, (void*)scalerBuf, b, true);

      /// TEST: print values from flash
      /* print_dbg("\r\n values in flash after write: "); */
      /* for(u32 i=0; i<(b>>2); ++i) { */
      /* 	s32* src = (s32*)(dst + (i << 2)); */
      /* 	print_dbg("\r\n 0x"); */
      /* 	print_dbg_hex((u32)(*src)); */
      /* } */
      /* print_dbg("\r\n first/last values in flash after write: "); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex((u32)(*((s32*)dst))); */
      /* print_dbg("\r\n 0x"); */
      /* print_dbg_hex((u32)(*((s32*)(dst + b - 4)))); */


    }
  }
}

// initialize buffer
void flash_bees_init(void) {
  scalerBuf = (s32*)alloc_mem(scalerMaxValues * 4);
}
