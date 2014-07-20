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
typedef wave *wavePointer;


//static function declarations
static wavePointer init(void);
static void fake_fread(volatile u8 *dst, u32 len, void *fp);


//external functions
void wavetables_init(void) {
    FL_DIR dirstat;
    struct fs_dir_ent dirent; //see fat_access.h
    
    int i = 0;
    tabList.num = 0;
    
    strcpy(tabList.path, TAB_PATH);
    
    if (fl_opendir(tabList.path, &dirstat))
    {        
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if (!(dirent.is_dir))
            {
                wavetables[i] = init();
                strcpy(tabList.path, TAB_PATH);
                **wavetables[i] = strcat(tabList.path, dirent.filename);
                print_dbg("\r\n wavetable path: ");
                print_dbg(**wavetables[i]);
                tabList.num++;
                i++;
            }
        }
        //fl_closedir(&dirstat);
    }
}


void files_load_wavetable(void) {
    void *fp = **wavetables[0];
    print_dbg("\r\n wavetable path: ");
    print_dbg(fp);
    volatile u8 *tbuf = 0; //WHAT SIZE, HOW TO DEFINE?
    u32 size = 0;
    u8 ret = 0;

app_pause();
    //open file
    fl_fopen(fp, "r");
    
    if (fp != NULL)
    {
        size = ((FL_FILE*)(fp))->filelength;
        fake_fread(tbuf, size, fp);
        fl_fclose(fp);
        
//        bfin_load_wavetable(); //DO I NEED TO TAILOR A FUNCTION TO TRANSFER A WAVETABLE?
        
    }
    else
    {
        ret = 0;
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


//static functions
wavePointer init(void) { //return pointer to chunk of memory to hold one wavetable file path
    return(wavePointer)alloc_mem(sizeof(wave));
}


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
