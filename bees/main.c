#include <stdio.h>
#include "../common/types.h"
#include "net.h"
#include "ui.h"

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
