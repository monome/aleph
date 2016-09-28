//files.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "delay.h"

#include "bfin.h"

#include "events.h"
#include "event_types.h"
#include "control.h"
#include "ctl.h"
#include "render.h"

#include "tracker.h"
#include "pages.h"

#include "app.h"
#include "filesystem.h" //includes fat_filelib.h
#include "flash.h"
#include "memory.h"
#include "print_funcs.h"

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"
#define WAV_PATH "/data/prgm/samples/"
#define PDX_PATH "/data/prgm/patterns/pdxd3s/"
#define PRGM_PATH "/data/prgm/patterns/prgm/"
#define CPS_PATH "/data/prgm/compositions/"
#define SCN_PATH "/data/prgm/scenes/"
#define MAX_NAME 16
#define MAX_PATH 64
#define MAX_PATH_1 (MAXPATH - 1)
#define DIR_LIST_NAME_BUF_SIZE 4096

#define N_BUFFERS 8                     //number of audio buffers
#define N_SAMPLES 128                   //number of samples
#define N_OFFSETS (N_BUFFERS + N_SAMPLES)

#define N_PATTERNS 32                   //number of patterns

#define AUX_SIZE 0xffff
#define SAMPLE_SIZE 0x7D000             //max size per sample (512KB)
#define SAMPLES_SIZE BFIN_SAMPLES_MAX_BYTES

#define BFIN_BUFFER_SIZE ((AUX_SIZE * 8) + SAMPLES_SIZE) //max size for all offsets

#define FILE_SIZE 1024                  //file size in bytes

//sample
typedef struct _prgmSample *prgmSampleptr;

typedef struct _prgmSample {
    u32 num;
    u32 offset;
    u32 loop;
    s32 offset_cut;
    s32 loop_cut;
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

//arrays holding file paths
char *pdx_path[N_PATTERNS];
char *prgm_path[N_PATTERNS];
char *cps_path[N_PATTERNS];
char *scn_path[N_PATTERNS];

//arrays holding file names
char *pdx_name[N_PATTERNS];
char *prgm_name[N_PATTERNS];
char *cps_name[N_PATTERNS];
char *scn_name[N_PATTERNS];

//number of files
u8 n_pdx;
u8 n_prgm;
u8 n_cps;
u8 n_scn;

//file state
u8 dirty_pdx;
u8 dirty_prgm;
u8 dirty_cps;
u8 dirty_scn;

//load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

//init samples
extern void samples_init(void);

//load sample
extern void files_load_sample(u8 n);

//init patterns
extern void patterns_init(void);

//init composition
extern void composition_init(void);

//init deck
extern void deck_init(void);

//init scene
void scene_init(void);

//load file, return 1 on success, 0 on failure
extern uint8_t files_load_nprgm(char *pattern[], u8 n);
extern uint8_t files_load_npdx(char *pattern[], u8 n);
extern uint8_t files_load_ncps(char *pattern[], u8 n);
extern uint8_t files_load_nscn(char *pattern[], u8 n);

//save file, return 1 on success, 0 on failure
extern uint8_t files_save_nprgm(char *pattern[], u8 n);
extern uint8_t files_save_npdx(char *pattern[], u8 n);
extern uint8_t files_save_ncps(char *pattern[], u8 n);
extern uint8_t files_save_nscn(char *pattern[], u8 n);


#endif 


