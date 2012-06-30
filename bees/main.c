
#include <stdio.h>
#include "../common/types.h"
#include "net.h"
#include "menu.h"
#include "ui.h"
#include "preset.h"
#include "audio.h"

int main(const int argc, const char** argv) {
  U8 run = 1; 
  
  net_init();
  ui_init();
  menu_init();
  preset_init();
  //  audio_init();

  // TEST:
  // create some dummy parameters
  net_add_param(0, "unit", 0.0, 1.0, 0.5);
  net_add_param(1, "big", 0.0, 10000.0, 500);
  net_add_param(2, "bipolar", -1.0, 1.0, 0.0);

  while (run != 0) {
    run = ui_loop();
  }

  // audio_deinit();
  preset_deinit();
  menu_deinit();
  ui_deinit();
  net_deinit();

  return 0;
}
