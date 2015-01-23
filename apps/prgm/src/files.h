//files.h
//aleph-prgm-avr32

#include "types.h"

#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"
#define WAV_PATH "/data/prgm/samples/"
#define MAX_NAME 16
#define MAX_PATH 64
#define MAX_PATH_1 (MAXPATH - 1)
#define DIR_LIST_NAME_BUF_SIZE 512

#define N_SAMPLES 32
#define SAMPLE_SIZE 0xBB800                 //16 seconds


//sample
typedef struct _prgmSample *prgmSampleptr;

typedef struct _prgmSample {
    u32 num;
    u32 offset;
    u32 size;
} prgmSample;

prgmSample *sample[N_SAMPLES];

//array holding sample file paths
char *sample_path[N_SAMPLES];

//array holding sample names
char *sample_name[N_SAMPLES];

//number of samples loaded: /data/prgm/samples
u8 n_samples;

//init samples
extern void samples_init(void);

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

//load samples, return 1 on success, 0 on failure
extern void files_load_samples(void);

#endif 


