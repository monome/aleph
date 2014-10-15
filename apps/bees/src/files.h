#ifndef _FILES_H_
#define _FILES_H_

#include "types.h"

// initialize filesystem navigation
extern void files_init(void);
// list all files (LS)
extern void files_list(void);

// create scenes dir if it doesn't exist
extern void files_check_scenes(void);
// create dsp dir if it doesn't exist
extern void files_check_dsp(void);

//----- dsp
// populate the dsp filelist with names and return count
// extern void files_scan_dsp(void);
// return dsp filename for given index
extern const volatile char* files_get_dsp_name(u8 idx);
// return count of dsp files
extern u8 files_get_dsp_count(void);
// load a blackfin executable by index
extern u8 files_load_dsp(u8 idx);
// load a blackfin executable by filename
// return 1 on success, 0 on failure
extern u8 files_load_dsp_name(const char* name);
// store dsp in flash, by index
// return 1 on success, 0 on failure
extern void files_store_default_dsp(u8 idx);
// store .ldr as default in internal flash, given name
extern void files_store_default_dsp_name(const char* name);

//----- scenes
// populate scene filelist with names and return count
// extern void files_scan_scene(void);
// return scene filename for given index
extern const volatile char* files_get_scene_name(u8 idx);
// return count of scene files
extern u8 files_get_scene_count(void);
// load scene by index
// return 1 on success, 0 on failure
extern u8 files_load_scene(u8 idx);
// load scene by filename
// return 1 on success, 0 on failure
extern u8 files_load_scene_name(const char* name);
extern u8 files_load_scene_path(const char* name);

// store scene by index
extern void files_store_scene(u8 idx);
// store scene by filename
extern void files_store_scene_name(const char* name /*, u8 ext */);


//----- scalers
// return dsp filename for given index
extern const volatile char* files_get_scaler_name(u8 idx);
// return count of dsp files
extern u8 files_get_scaler_count(void);
// load scaler by filename
// return 1 on success, 0 on failure
extern u8 files_load_scaler_name(const char* name, s32* dst, u32 dstSize);

//----- param descriptors
// search for named .dsc file and load into network param desc memory
extern u8 files_load_desc(const char* name);

// check for extension
extern bool check_ext(char* str, const char* ext );

// strip extension from the end of a string
extern bool strip_ext(char* str);

#endif // header guard 
