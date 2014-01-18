#include <string.h>

#include "print_funcs.h"

#include "bfin.h"
#include "control.h"

// request a parameter change.
extern u8 ctl_param_change(u32 idx, u32 val) {
#if 1
#else
  bfin_wait_ready();
  bfin_set_param(idx, val);
  return 0;
#endif
}

