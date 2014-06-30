//prgm
//aleph-avr32

#include <string.h>

//asf
#include "delay.h"
#include "print_funcs.h"

//avr32
#include "app.h"
#include "bfin.h"
#include "control.h"
#include "interrupts.h"

//common
#include "fix.h"
#include "types.h"
#include "param_common.h"

//prgm
#include "ctl.h"

#define DSP_NAME "aleph-prgm"

// get parameter report from DSP
u8 ctl_report_parameters(void) {
    volatile char buf[64];
    u32 numParams;
    
    bfin_get_num_params(&numParams);
    
    print_dbg("\r\nnumparams: ");
    print_dbg_ulong(numParams);

#if 1
#else
    volatile ParamDesc pdesc;
    u16 i;

    if(numParams > 0) {
        for(i=0; i<numParams; i++) {
            bfin_get_param_desc(i, &pdesc);
        }
            print_dbg("\r\n got pdesc : ");
            print_dbg((const char*)pdesc.label);
        }
    } else {
            print_dbg("\r\n report_params fail (0)");
            return 0;
    }
#endif

    delay_ms(100);
    
    print_dbg("\r\n checking module label ");
    // check module label
    bfin_get_module_name(buf);
    
    delay_ms(10);
    
    print_dbg("\r\n bfin module name: ");
    print_dbg((const char*)buf);
    if(strcmp((const char*)buf, DSP_NAME)) {
        print_dbg( "\r\n report_params fail (module name mismatch)" );
        return 0;
    } else {
        return 1;
    }
}
