#include <string.h>

#include "print_funcs.h"

#include "bfin.h"
#include "control.h"
#include "delay.h"

// request a parameter change.
extern u8 ctl_param_change(u32 idx, u32 val) {
    bfin_wait_ready();
    delay_ms(1);
    bfin_set_param(idx, val);
    return 0;
}
