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

//buffer for wav data (wav curve)
//static s32 wavbuf;

//file directory
typedef struct _dirList {
    char path[MAX_PATH];
    volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
    u32 num;
} dirList_t;

//static function declarations
static void dsp_read(volatile u8 *dst, u32 len, void *fp);

//static functions
void dsp_read(volatile u8 *dst, u32 len, void *fp) { //fread: no size arg
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

//external functions
u8 files_load_dsp(void) {
    void *fp;
    u32 size = 0;
    u8 ret;
    
    delay_ms(10);
    
    app_pause();
    
    fp = fl_fopen(DSP_PATH, "r");
    
    if(fp != NULL) {
        size = ((FL_FILE*)(fp))->filelength;
        dsp_read(bfinLdrData, size, fp);
        
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

u8 files_load_wav(void) {
/*
    u8 status;
    u32 i;
    u32 size = 0;
    
    app_pause();
    
    //  open wav file
    void* fp = fl_fopen(WAV_PATH);
    print_dbg("\r\n found file...");
    
    size = ((FL_FILE*)(fp))->filelength;
    print_dbg("\r\n file size in bytes...");
    print_dbg_ulong(size);

    for
    //  send begin transfer command to bfin
    //size argument?!
    bfin_start_wavtransfer(void);
    
    for (i=0; i<size; i+=4)
    {
        
    //  read to buffer
    dsp_read(&wavbuf, 4, fp);
    
    //  send buffer transfer command to bfin
    bfin_transfer_wavbytes(wavbuf);
    }
    
    app_resume();
*/
    return 0;
}
