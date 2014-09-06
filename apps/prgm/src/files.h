//files.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

//#include "types.h" ?!?
//#include <types.h> ?!?

//#define DEFAULT_LDR "/mod/aleph-prgm.ldr"
//#define DEFAULT_DSC "/mod/aleph-prgm.dsc"

#define BFIN_WAVE_MAX_BYTES 0x10000

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"
#define TAB_PATH "/data/prgm/wavetables/"
#define MAX_PATH 64
#define MAX_PATH_1 (MAXPATH - 1)
#define DIR_LIST_NAME_BUF_SIZE 512

#define MAX_TABLES 32
#define WAVE_SHAPE_NUM 2
#define WAVE_TAB_SIZE 512

//array holding wavetable file paths
char *waves[MAX_TABLES];

//number of files found in folder /data/prgm/wavetables/
s32 numwaves;

//initialize list of wavetable files
extern void wavetables_init(void);

//load next wavetable file to temporary buffer
extern void files_load_wavetable(s32 idx);

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

#endif 


