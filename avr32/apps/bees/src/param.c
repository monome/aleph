/* param.c
 * bees
 * aleph
 */


// common
#include "param_common.h"
// avr32
#include "print_funcs.h"
// bees
#include "ui.h"
#include "net.h"
#include "net_protected.h"
#include "menu.h"
#include "param.h"
#include "bfin.h"


// get value for param at given idx
io_t get_param_value(u32 idx) {
  return (io_t)(net->params[idx].data.value.asInt); 
}

// get preset-enabled flag for param at given idx
u8 get_param_preset(u32 idx) {
  return net->params[idx].preset;
}


// get name (label) for param at given idx
const char* get_param_name(u32 idx) {
  return net->params[idx].desc.label;
}

// set value for param at given idx
//-- see also net_set_in_value()
void set_param_value(u32 idx, io_t val) {
  /* print_dbg("\r\n setting param : idx 0x"); */
  /* print_dbg_hex(idx);   */
  /* print_dbg(" , label: "); */
  /* print_dbg(net->params[idx].desc.label); */
  /* print_dbg(" , val: 0x"); */
  /* print_dbg_hex((u32)val); */
  /* print_dbg(" , min: 0x"); */
  /* print_dbg_hex((u32)net->params[idx].desc.min); */
  /* print_dbg(" , max: 0x"); */
  /* print_dbg_hex((u32)net->params[idx].desc.max); */

  if(val > net->params[idx].desc.max) {
    val = net->params[idx].desc.max;
  }
  if(val < net->params[idx].desc.min) {
    val = net->params[idx].desc.min;
  }
  net->params[idx].data.value.asInt = val;
  net->params[idx].data.changed = 1;
  bfin_set_param(idx, val);
}
