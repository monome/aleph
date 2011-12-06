#include <stdio.h>
#include "compiler.h"
#include "ctl_interface.h"

static void print_ops (void) {
  U8 i;
  for(i=0; i<ctl_num_ops(); i++) {
    printf("%d) %s\n", i, ctl_op_name(i));
  }
}
  
static void print_inputs(void) {
  U8 i, j;
  for(i=0; i<ctl_num_ins(); i++) {
    printf("%d) %s\n", i, ctl_in_name(i));
  } 
}

int main(const int argc, const char** argv) {
  S16 sw_idx;
  S32 sw_val;

  sw_val = 1;

  ctl_net_init();
  sw_idx = ctl_add_op(eOpSwitch);

  ctl_go(sw_idx, &sw_val);
}
