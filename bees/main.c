#include <stdio.h>
#include "../common/types.h"
#include "net.h"
#include "menu.h"
#include "ui.h"
#include "preset.h"

int main(const int argc, const char** argv) {
  U8 run = 1; 
  
  net_init();
  ui_init();
  menu_init();
  preset_init();

  while (run != 0) {
    run = ui_loop();
  }
  
  preset_deinit();
  menu_deinit();
  ui_deinit();
  net_deinit();
  return 0;
}
