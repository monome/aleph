//files.h
//aleph-prgm-avr32

#include "types.h"

#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"
#define WAV_PATH "/data/prgm/samples/"
#define MAX_PATH 64
#define MAX_PATH_1 (MAXPATH - 1)
#define DIR_LIST_NAME_BUF_SIZE 512

#define MAX_SAMPLES 32

//array holding wavetable file paths
char *samples[MAX_SAMPLES];

//number of files found in folder /data/prgm/samples
s32 numsamples;

//initialize list of samples
extern void samples_init(void);

//initialize buffer for samples
extern void samplebuffer_init(void);

//load next sample file to temporary buffer
extern void files_load_samples(u32 idx);

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

#endif 


