/* param.c
 * bees
 * aleph
 */
#include "ui.h"
#include "net.h"
#include "net_protected.h"
#include "menu.h"
#include "param.h"

// enable to print debug values
#define PARAM_PRINT_TEST 0

// get value for param at given idx
f32 get_param_value(u32 idx) {
  return net.params[idx].fval;
}

// get preset-enabled for param at given idx
u8 get_param_preset(u32 idx) {
  return net.params[idx].preset;
}

/////////
////// TODO: flexible / accurate param scaling using tables!
////////

// set value for param at given idx
void set_param_value(u32 idx, s32 val) {
  f32 fval;
#if PARAM_PRINT_TEST
  static char buf[SCREEN_W];
#endif 

  //  if (val < net.params[idx].min) { val = net.params[idx].min; }
  //  if (val > net.params[idx].max) { val = net.params[idx].max; }

  // net.params[idx].val = val;
  net.params[idx].val = val;
  //////
  // DEBUG
  fval = (f32)val * PARAM_MAX_RF;
  fval += 1.f;
  fval *= 0.5f;
  fval *= (net.params[idx].max - net.params[idx].min);
  fval += net.params[idx].min;
  //////
  net.params[idx].fval = fval;
  
  /*
  if (fval < net.params[idx].min) { fval = net.params[idx].min; }
  if (fval > net.params[idx].max) { fval = net.params[idx].max; }
  */

  param_feedback(idx);

#if PARAM_PRINT_TEST
  snprintf(buf, SCREEN_W, "setting parameter value %d at index%d", (int)(val), (int)(net.params[idx].idx));
  ui_print(SCREEN_H, 0, buf, 0);
#endif

}
