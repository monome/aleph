#include <stdio.h>
#include "compiler.h"
#include "ctl_interface.h"
#include "ui.h"

int main(const int argc, const char** argv) {
  U8 run = 1; 
  
  ctl_net_init();
  ui_init();
  
  while (run != 0) {
    run = ui_loop();
  }
  
  return 0;
}
