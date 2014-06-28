//prgm
//aleph-avr32

// standard lib headers
#include <string.h>

// for delay_ms()
#include "delay.h"
// for print_dbg()
#include "print_funcs.h"

//--- -aleph-avr32 headers
// includes pause/resume
#include "app.h"
// control change functions. 
// (as of now, just a thin wrapper around blackfin spi comm layer.)
#include "control.h"

//---- aleph/common headers
// fixed-point representation
#include "fix.h"
// parameter types
#include "param_common.h"

//--- custom app headers
#include "app_timers.h"
#include "ctl.h"
#include "files.h"
#include "render.h"
#include "util.h"


// get param value
//u32 ctl_get_param(u32 pid) {
//    return (const u32)ParamVal[pid];
//}

//void ctl_eParamVal(u32 pid, fract32 val) {
//    ParamVal[pid] = val;
//    ctl_param_change(pid, val); //declared in control.h
//}

void ctl_eParamFreq0(u32 pid, fract32 val) {
    ctl_param_change(eParamFreq0 + pid, val);
}

