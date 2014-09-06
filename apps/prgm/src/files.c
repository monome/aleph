//files.c
//aleph-prgm-avr32

// std
#include <stdlib.h>
#include <string.h>

// asf
#include "delay.h"
#include "print_funcs.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "filesystem.h" //includes fat_filelib.h
#include "flash.h"
#include "memory.h"

//prgm
#include "files.h"
#include "render.h"


typedef struct _dirList {
    char path[MAX_PATH];
    volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
    u32 num;
} dirList_t;

static dirList_t tabList;

//static function declarations
static void fake_fread(volatile u8 *dst, u32 len, void *fp);
static char *wave_filepath(s32 n);

//static functions
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


char *wave_filepath(s32 n) {
    return (n < 0 || n > numwaves) ? waves[0] : waves[n];
}


//external functions
void wavetables_init(void) {
    FL_DIR dirstat;
    struct fs_dir_ent dirent; //see fat_access.h
    
    s32 i = 0;
    tabList.num = 0;
    numwaves = 0;
    
    strcpy(tabList.path, TAB_PATH);
    
    if (fl_opendir(tabList.path, &dirstat))
    {        
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if (!(dirent.is_dir))
            {
                waves[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                strcpy(tabList.path, TAB_PATH);
                strcat(tabList.path, dirent.filename);
                strcpy(waves[i], tabList.path);
                print_dbg("\r\n waves[i]: ");
                print_dbg(waves[i]);
                tabList.num++;
                i++;
                numwaves++;
            }
        }
        //fl_closedir(&dirstat);
    }
    bfinWaveData = alloc_mem(BFIN_WAVE_MAX_BYTES);
    for(i=0; i<numwaves; i++) { bfinWaveData[i] = 0; }
    bfinWaveSize = 0;
}


void files_load_wavetable(s32 idx) {
    void *fp;
    u32 size = 0;
    
    app_pause();
    
    if (idx < numwaves) {
        fp = fl_fopen(wave_filepath(idx), "r");
        print_dbg("\r\n wave_filepath(idx) ");
        print_dbg(wave_filepath(idx));
        
        if (fp != NULL)
        {
            size = ((FL_FILE*)(fp))->filelength;
            fake_fread(bfinWaveData, size, fp);
            fl_fclose(fp);
            bfinWaveSize = size;
            print_dbg("\r\n bfinWaveSize ");
            print_dbg_ulong(bfinWaveSize);
        }
        else ;
    }
    else
    {
        print_dbg("\r\n idx out of bounds");
    }
    app_resume();
}


u8 files_load_dsp(void) {
    void *fp;
    u32 size = 0;
    u8 ret;
    
    delay_ms(10);
    
    app_pause();
    
    fp = fl_fopen(DSP_PATH, "r");
    print_dbg("\r\n found file...");
    
    if(fp != NULL) {
        size = ((FL_FILE*)(fp))->filelength;
        print_dbg("\r\n found file, loading dsp: ");
        fake_fread(bfinLdrData, size, fp);
        
        fl_fclose(fp);
        bfinLdrSize = size;
        
        if(bfinLdrSize > 0) {
            print_dbg("\r\n loading bfin from buf");
            
            bfin_load_buf();
            
            print_dbg("\r\n finished load");
            //            print_dbg("\r\n loading parameter descriptor file...");
            //            ret = files_load_desc(name);
            
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
