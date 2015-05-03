//files.h
//aleph-prgm-avr32

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "delay.h"

#include "bfin.h"

#include "events.h"
#include "event_types.h"
#include "control.h"
#include "ctl.h"

#include "app.h"
#include "filesystem.h" //includes fat_filelib.h
#include "flash.h"
#include "memory.h"
#include "print_funcs.h"


#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"
#define WAV_PATH "/data/prgm/samples/"
#define MAX_NAME 16
#define MAX_PATH 64
#define MAX_PATH_1 (MAXPATH - 1)
#define DIR_LIST_NAME_BUF_SIZE 512

#define N_BUFFERS 8                     //number of audio buffers
#define N_TABLES 0                      //number of wavetables
#define N_SAMPLES 128                   //number of samples
#define N_OFFSETS (N_BUFFERS + N_TABLES + N_SAMPLES)

#define REC_SIZE 0xBB800                //recording buffer maximum size (16 seconds)
#define AUX_SIZE 0x2EE00                //aux buffer maximum size (4 seconds)
#define ENV_SIZE 1024                   //max size per envelope (1024 bytes)
#define SAMPLE_SIZE 0x7D000             //max size per sample (512KB)
#define SAMPLES_SIZE BFIN_SAMPLES_MAX_BYTES

#define BFIN_BUFFER_SIZE ((AUX_SIZE * 8) + SAMPLES_SIZE) //max size for all offsets (0x01e84800 bytes?)

//sample
typedef struct _prgmSample *prgmSampleptr;

typedef struct _prgmSample {
    u32 num;
    u32 offset;
    u32 loop;
} prgmSample;

prgmSample *sample[N_OFFSETS];

//array holding sample file paths
char *sample_path[N_OFFSETS];

//array holding sample names
char *sample_name[N_OFFSETS];

//number of samples loaded: /data/prgm/samples
u8 n_samples;
u8 smpl;
u32 idx8;
u32 idx32;

//init samples
extern void samples_init(void);
//extern void wavetables_init(void);

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

//load samples, return 1 on success, 0 on failure
extern void files_load_sample(u8 n);

#endif 


