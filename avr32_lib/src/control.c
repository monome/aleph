#include <string.h>

#include "print_funcs.h"

#include "bfin.h"
#include "control.h"

// request a parameter change.
u8 ctl_param_change(u32 pos, u32 idx, u32 val) {
  bfin_wait_ready();
  bfin_set_sqparam(pos, idx, val);
  return 0;
}
