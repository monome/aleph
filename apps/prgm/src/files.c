//prgm
//aleph-avr32

// std
#include <stdlib.h>
#include <string.h>

// asf
#include "delay.h"
#include "print_funcs.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "filesystem.h"
#include "flash.h"
#include "memory.h"

//prgm
#include "files.h"
#include "render.h"

#define DSP_PATH "/mod/aleph-prgm.ldr"
#define DSC_PATH "/mod/aleph-prgm.dsc"

// fread: no size arg
static void fake_fread(volatile u8 *dst, u32 len, void *fp) {
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

// search for specified dsp file and load it
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


/*
// set initial parameters
void ctl_init_params(void) {
    // dacs at 0
    ctl_param_change(eParam_dac0, 0);
    ctl_param_change(eParam_dac1, 0);
    ctl_param_change(eParam_dac2, 0);
    ctl_param_change(eParam_dac3, 0);
    /// slew at ???
    //// work on inputs standardization
    ctl_param_change(eParam_slew0, 0x7fff0000);
    ctl_param_change(eParam_slew1, 0x7fff0000);
    ctl_param_change(eParam_slew2, 0x7fff0000);
    ctl_param_change(eParam_slew3, 0x7fff0000);
    
}
*/


/*
// search for named .dsc file and load into network param desc memory
u8 files_load_dsp_parameters(void) {
    char path[64] = DSP_PATH;
    void *fp;
    int nparams = -1;
    // word buffer for 4-byte unpickling
    u8 nbuf[4];
    // buffer for binary blob of single descriptor
    u8 dbuf[PARAM_DESC_PICKLE_BYTES];
    // unpacked descriptor

    ParamDesc desc;
    int i;
    u8 ret = 0;
    
    app_pause();
    
    strcat(path, name);
    strip_ext(path);
    strcat(path, ".dsc");
    
    print_dbg("\r\n  opening .dsc file at path: ");
    print_dbg(path);
    
    fp = fl_fopen(path, "r");
    if(fp == NULL) {
        print_dbg("... error opening .dsc file.");
        print_dbg(path);
        ret = 1;
    } else {
        
        // get number of parameters
        fake_fread(nbuf, 4, fp);
        unpickle_32(nbuf, (u32*)&nparams); 
        
        /// loop over params
        if(nparams > 0) {
            net_clear_params();
            //    net->numParams = nparams;
            
            for(i=0; i<nparams; i++) {
                //  FIXME: a little gross,
                // to be interleaving network and file manipulation like this...
                ///....
                // read into desc buffer
                fake_fread(dbuf, PARAM_DESC_PICKLE_BYTES, fp);
                // unpickle directly into network descriptor memory
                pdesc_unpickle( &desc, dbuf );
                // copy descriptor to network and increment count
                net_add_param(i, (const ParamDesc*)(&desc));     
                
            }
        } else {
            print_dbg("\r\n error: crazy parameter count from descriptor file.");
            ret = 1;
        }
    }
    fl_fclose(fp);
    app_resume();
    return ret;
}
*/

/*
u8 files_load_scene_name(const char* name) {
    void* fp;
    u32 size = 0;
    u8 ret = 0;
    
    //// ahhhhh, i see.. 
    /// this is overwriting the descriptor in sceneData as well as the serialized blob.
    /// woud be fine, except it fucks up the comparison later.
    /// for now, let's do this ugly-ass workaround.
    
    char oldModuleName[MODULE_NAME_LEN];
    /// store extant module name
    strncpy(oldModuleName, sceneData->desc.moduleName, MODULE_NAME_LEN);
    
    app_pause();
    
    fp = list_open_file_name(&sceneList, name, "r", &size);
    
    if( fp != NULL) {	  
        print_dbg("\r\n reading binary into sceneData serialized data buffer...");
        fake_fread((volatile u8*)sceneData, sizeof(sceneData_t), fp);
        print_dbg(" done.");
        
        /// copy old name back to descriptor field... dumb dumb dumb.
        strncpy(sceneData->desc.moduleName, oldModuleName, MODULE_NAME_LEN);
        
        fl_fclose(fp);
        scene_read_buf();
        
        // try and load dsp module indicated by scene descriptor
        //// DUDE! NO!!! scene does this. when did this happen!
        //// probably snuck in in some merge.
        //    ret = files_load_dsp_name(sceneData->desc.moduleName);
    } else {
        print_dbg("\r\n error: fp was null in files_load_scene_name \r\n");
        ret = 0;
    } 
    
    app_resume();
    return ret;
}
*/