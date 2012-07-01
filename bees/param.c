/* param.c
 * bees
 * aleph
 */
#include "ui.h"
#include "net.h"
#include "net_protected.h"
#include "param.h"

// enable to print debug values
#define PARAM_PRINT_TEST 1

// get value for param at given idx
f32 get_param_value(u32 idx) {
  return net.params[idx].val;
}

// get preset-enabled for param at given idx
u8 get_param_preset(u32 idx) {
  return net.params[idx].preset;
}

// set value for param at given idx
void set_param_value(u32 idx, f32 val) {

#if PARAM_PRINT_TEST
  static char buf[SCREEN_W];
#endif 

  if (val < net.params[idx].min) { val = net.params[idx].min; }
  if (val > net.params[idx].max) { val = net.params[idx].max; }

  net.params[idx].val = val;

#if PARAM_PRINT_TEST
  snprintf(buf, SCREEN_W, "setting parameter value %d at index%d", (int)(val), (int)(net.params[idx].idx));
  ui_print(SCREEN_H, 0, buf, 0);
#endif

}
