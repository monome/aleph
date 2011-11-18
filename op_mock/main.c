#include <stdio.h>
#include "ctl_interface.h"

int main(const int argc, const char** argv) {
  S16 sw_idx;
  S32 sw_val;

  sw_val = 1;

  ctl_net_init();
  sw_idx = ctl_add_op(eOpSwitch);

  ctl_go(sw_idx, &sw_val);
}
