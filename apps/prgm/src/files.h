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

#define MAX_SAMPLES 8

//array holding sample file paths
char *samples[MAX_SAMPLES];

//number of samples/files found in folder /data/prgm/samples
u8 numSamples;

//alloc memory for a temporary loading buffer
extern void alloc_sample_buffer(void);

//alloc memory for, and fill an array with sample file paths
extern void alloc_sample_paths(void);

//load a sample file to temporary buffer
extern void files_load_sample(u8 idx);

//load all samples
extern void files_load_samples(void);

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

#endif 


