/* param.c
 * bees
 * aleph
 */


// common
#include "param_common.h"
// avr32
#include "control.h"
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
  //  bfin_set_param(idx, val);
  ctl_param_change(idx, (u32)val);
}


// get param descriptors and initial values from DSP
void report_params(void) {
  volatile ParamDesc pdesc;
  u32 numParams;
  u8 i;

  bfin_get_num_params(&numParams);
  print_dbg("\r\nnumparams: ");
  print_dbg_ulong(numParams);

  if(numParams == 255) {
    print_dbg("\r\n bfin reported too many parameters; something went wrong.");
    return;
  }

  if(numParams > 0) {
    net_clear_params();
    for(i=0; i<numParams; i++) {
      bfin_get_param_desc(i, &pdesc);

      /// FIXME: arg, this belongs only in BEES
      net_add_param(i, &pdesc);     
      
      print_dbg("\r\n got pdesc : ");
      print_dbg((const char* )pdesc.label);
    }
  }

  /// TODO: get values...

  //  bfin_enable();
}
