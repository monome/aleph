#include <string.h>

#include "print_funcs.h"

#include "delay.h"
#include "bfin.h"
#include "control.h"

// request a parameter change.
u8 ctl_param_change(u32 idx, u32 val) {
  bfin_wait_ready();
  bfin_set_param(idx, val);
  return 0;
}

//transfer wavetable
u8 ctl_wavetable_change(void) {
    bfin_wait_ready();
    bfin_load_wavbuf();
    print_dbg("\r\n bfin_load_wavbuf() done... ");
    return 0;
}
