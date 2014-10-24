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
