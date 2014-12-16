//files.c
#include "files.h"

#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "delay.h"
#include "filesystem.h" //includes fat_filelib.h
#include "flash.h"
#include "memory.h"
#include "print_funcs.h"

//file directory
typedef struct _dirList {
    char path[MAX_PATH];
    volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
    u32 num;
} dirList_t;

static dirList_t sampleList;


//static function declarations
static void fake_fread(volatile u8 *dst, u32 len, void *fp);
static char *sample_filepath(s32 n);


//static function definitions
void fake_fread(volatile u8 *dst, u32 len, void *fp) { //fread: no size arg
    u32 n = 0;
#if 0
    fl_fread(&dst, 1, len, fp);
#else
    while(n < len) {
        *dst = fl_fgetc(fp);
        n++;
        dst++;
    }
#endif
}

char *sample_filepath(s32 n) {
    return (n < 0 || n > numSamples) ? samples[0] : samples[n];
}


//external functions
void alloc_sample_paths(void) {
    FL_DIR dirstat;
    struct fs_dir_ent dirent; //see fat_access.h
    
    s32 i = 0;
    sampleList.num = 0;
    numSamples = 0;
    
    strcpy(sampleList.path, WAV_PATH);
    
    if (fl_opendir(sampleList.path, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if (!(dirent.is_dir))
            {
                samples[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                strcpy(sampleList.path, WAV_PATH);
                strcat(sampleList.path, dirent.filename);
                strcpy(samples[i], sampleList.path);
                print_dbg("\r\n samples[i]: ");
                print_dbg(samples[i]);
                sampleList.num++;
                i++;
                numSamples++;
            }
        }
        //fl_closedir(&dirstat);
    }
}

void alloc_sample_buffer(void) {
    bfinSampleData = (u8*)alloc_mem(BFIN_SAMPLE_MAX_BYTES * sizeof(u8));
}

void files_load_sample(u8 idx) {
    void *fp;
    u32 size = 0;
    
app_pause();
    fp = fl_fopen(sample_filepath(idx), "r");
    print_dbg("\r\n sample_filepath(idx) ");
    print_dbg(sample_filepath(idx));
    
    if (fp != NULL)
    {
        size = ((FL_FILE*)(fp))->filelength;
        fake_fread(bfinSampleData, size, fp);
        fl_fclose(fp);
        
        bfinSampleSize = size / sizeof(s32);
        print_dbg("\r\n bfinSampleSize ");
        print_dbg_ulong(bfinSampleSize);

//        bfin_fill_buffer(idx, bfinSampleSize, (const s32*)bfinSampleData);
        bfin_fill_buffer(idx, bfinSampleSize, (s32*)bfinSampleData);
        print_dbg("\r\n bfin_fill_buffer finished... ");
    }
    
    else
    {
        print_dbg("\r\n idx out of bounds");
    }
    
app_resume();
}

void files_load_samples(void) {
    u32 i;
    for (i=0; i<numSamples; i++)
    {
        files_load_sample(i);
    }
}

u8 files_load_dsp(void) {
    void *fp;
    u32 size = 0;
    u8 ret;
    
    delay_ms(10);
    
    app_pause();
    
    fp = fl_fopen(DSP_PATH, "r");
    
    if(fp != NULL) {
        size = ((FL_FILE*)(fp))->filelength;
        fake_fread(bfinLdrData, size, fp);
        
        fl_fclose(fp);
        bfinLdrSize = size;
        
        if(bfinLdrSize > 0) {
            bfin_load_buf();
            ret = 1;
        } else {
            print_dbg("\r\n bfin ldr size was <=0, aborting");
            ret = 0;
        }
    } else {
        print_dbg("\r\n error: fp was null in files_load_dsp_name \r\n");
        ret = 0;
    }
    
    app_resume();
    return ret;
}
