#include <stdio.h>
#include "pages.h"
#include "net.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"
#include "types.h"
#include "ui.h"

//ctlnet_t ctlnet;

// main function
int main(void) { //const int argc, const char** argv) {
  u8 run = 1; 
  
  //  net_init(&ctlnet);
  net_init();
  ui_init();
  pages_init();
  preset_init();
  scene_init();

  // TEST:
  // create some dummy parameters
  /* net_add_param(0, "unit", 0.0, 1.0, 0); */
  /* net_add_param(1, "big", 0.0, 10000.0, 0); */
  /* net_add_param(2, "bipolar", -1.0, 1.0, 0); */

  while (run != 0) {
    run = ui_loop();
  }

  scene_deinit();
  preset_deinit();
  pages_deinit();
  ui_deinit();
  net_deinit();

  return 0;
}
