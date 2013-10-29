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
#include "pages.h"
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

      net_add_param(i, (const ParamDesc*)&pdesc);     
      
      print_dbg("\r\n got pdesc : ");
      print_dbg((const char* )pdesc.label);
    }
  }

  /// TODO: get values...
  /// let the user decide.
  // bfin_enable();
}


// pickle / unpickle
u8* param_pickle(pnode_t* pnode, u8* dst) {
  //  u32 i;
  /// wasting some space to preserve 4-byte alignment
  // store idx
  dst = pickle_32((u32)pnode->idx, dst);
  // store value
  dst = pickle_32(pnode->data.value.asUint, dst);
  // store preset-inclusion 
  dst = pickle_32((u32)(pnode->preset), dst);
  // store descriptor
  
  return dst;
}

const u8* param_unpickle(pnode_t* pnode, const u8* src) {
  u32 val;
  // load idx
  src = unpickle_32(src, &val);
  pnode->idx = (u8)val;
  // load value
  src = unpickle_32(src, &(pnode->data.value.asUint));
  // load preset-inclusion 
  src = unpickle_32(src, &val);
  pnode->preset = (u8)val;
  // load descriptor
  // ... 
  return src;
}

// pickle/unpickle for param descriptors 
// (struct definition in param_common.h
u8* pdesc_pickle(ParamDesc* pdesc, u8* dst) {
  u8 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    *dst++ = pdesc->label[i];
  }
  // store unit string
  for(i=0; i<PARAM_UNIT_LEN; ++i) {
    *dst++ = pdesc->unit[i];
  }
  // store type
  *dst++ = pdesc->type;
  // store min
  dst = pickle_32(pdesc->min, dst);
  // store max
  dst = pickle_32(pdesc->max, dst);
  return dst;
}

const u8* pdesc_unpickle(ParamDesc* pdesc, const u8* src) {
  u8 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    pdesc->label[i] = *src++;
  }
  // store unit string
  for(i=0; i<PARAM_UNIT_LEN; ++i) {
    pdesc->unit[i] = *src++;
  }
  // store type
  pdesc->type = *src++;
  // store min
  src = unpickle_32(src, (u32*)&(pdesc->min));
  // store max
  src = unpickle_32(src, (u32*)&(pdesc->max));
  return src;
}
