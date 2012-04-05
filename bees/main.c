#include <stdio.h>
#include "compiler.h"
#include "ctlnet_interface.h"
#include "ui.h"

static void printInputs(void) {
  //  ctl_op_nameet
}

int main(const int argc, const char** argv) {
  U8 run = 1; 
  
  net_init();
  ui_init();
  
  while (run != 0) {
    run = ui_loop();
  }
  
  ui_deinit();
  net_deinit();
  return 0;
}
