/* param.c
 * bees
 * aleph
 */

#include "ui.h"
#include "net.h"
#include "net_protected.h"
#include "param.h"

// set value for param at given idx
void param_set(u32 idx, f32 val) {

  static char buf[SCREEN_W];

  if (val < net.params[idx].min) { val = net.params[idx].min; }
  if (val > net.params[idx].max) { val = net.params[idx].max; }

  net.params[idx].val = val;

  snprintf(buf, SCREEN_W, "setting parameter value %d at index%d", (int)(val), (int)(net.params[idx].idx));
  ui_print(SCREEN_H, 0, buf, 0);

}
