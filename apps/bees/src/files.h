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
extern void files_scan_dsp(void);
// return dsp filename for given index
extern const volatile char* files_get_dsp_name(u8 idx);
// return count of dsp files
extern u8 files_get_dsp_count(void);
// load a blackfin executable by index
extern void files_load_dsp(u8 idx);
// load a blackfin executable by filename
extern void files_load_dsp_name(const char* name);
// store dsp in flash, by index
extern void files_store_default_dsp(u8 idx);


//----- scenes
// populate scene filelist with names and return count
extern void files_scan_scene(void);
// return dsp filename for given index
extern const volatile char* files_get_scene_name(u8 idx);
// return count of dsp files
extern u8 files_get_scene_count(void);
// load scene by index
extern void files_load_scene(u8 idx);
// load scene by filename
extern void files_load_scene_name(const char* name);
// store scene by index
extern void files_store_scene(u8 idx);
// store scene by filename
extern void files_store_scene_name(const char* name);

// store default scene in flash, by index
extern void files_store_default_scene(u8 idx);

#endif // header guard 
