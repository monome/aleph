//files.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

//#include "types.h" ?!?
//#include <types.h> ?!?

//#define DEFAULT_LDR "/mod/aleph-prgm.ldr"
//#define DEFAULT_DSC "/mod/aleph-prgm.dsc"

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"
#define TAB_PATH "/data/prgm/wavetables/"
#define MAX_PATH 64
#define MAX_PATH_1 (MAXPATH - 1)
#define DIR_LIST_NAME_BUF_SIZE 4096

#define MAX_TABLES 32
#define WAVE_SHAPE_NUM 2
#define WAVE_TAB_SIZE 512

//arrays holding wavetable data
typedef char *wave[MAX_PATH];
wave *wavetables[MAX_TABLES];

//initialize list of wavetable files
extern void wavetables_init(void);

//load next wavetable file to temporary buffer
extern void files_load_wavetable(void);

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

#endif 


