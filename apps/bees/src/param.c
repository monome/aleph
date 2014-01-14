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
  return (io_t)(net->params[idx].data.value); 
}

// get preset-enabled flag for param at given idx
/* u8 get_param_preset(u32 idx) { */
/*   return net->params[idx].preset; */
/* } */


// get name (label) for param at given idx
const char* get_param_name(u32 idx) {
  return net->params[idx].desc.label;
}

// set value for param at given idx
//-- see also net_set_in_value()
// return sign of clamping operation, if clamped
void set_param_value(u32 idx, io_t val) {
  
  s32 scaled = scaler_get_value( &(net->params[idx].scaler), val);

  /* print_dbg("\r\n set_param_value, index: "); */
  /* print_dbg_ulong(idx); */

  /* print_dbg(" , value: 0x"); */
  /* print_dbg_hex(val); */

  /* print_dbg(" , scaled: 0x"); */
  /* print_dbg_hex(scaled); */
    
  /* if(val > net->params[idx].desc.max) { */
  /*   val = net->params[idx].desc.max; */
  /* } */
  /* if(val < net->params[idx].desc.min) { */
  /*   val = net->params[idx].desc.min; */
  /* } */

  net->params[idx].data.value = val;
  net->params[idx].data.changed = 1;

  // scale
  ctl_param_change(idx, scaled );
  //  ctl_param_change(idx, net->params[idx].data.value);
}


// pickle / unpickle
u8* param_pickle(pnode_t* pnode, u8* dst) {
  u32 val;
  /// wasting some space to preserve 4-byte alignment
  // store idx
  dst = pickle_32((u32)pnode->idx, dst);

  // print_dbg("\r\n pickling param node, value: 0x");
  // print_dbg_hex(pnode->data.value.asUint);

  // store value
  val = (u32)(pnode->data.value);
  dst = pickle_32(val, dst);

  // store preset-inclusion 
  //  dst = pickle_32((u32)(pnode->preset), dst);
  // store descriptor
  dst = pdesc_pickle(&(pnode->desc), dst);
  return dst;
}

const u8* param_unpickle(pnode_t* pnode, const u8* src) {
  u32 val;
  // load idx-
  src = unpickle_32(src, &val);

  // print_dbg("\r\n unpickled param index: ");
  // print_dbg_ulong(val);

  pnode->idx = (u8)val;
  // load value
  
  src = unpickle_32(src, &val);
  pnode->data.value = (ParamValue)val;
  //  src = unpickle_32(src, &(pnode->data.value.asUint));

  // print_dbg("\r\n unpickled param value: ");
  // print_dbg_ulong(val);

  // load preset-inclusion 
  //  src = unpickle_32(src, &val);

  // print_dbg("\r\n unpickled param preset flag: ");
  // print_dbg_ulong(val);

  //  pnode->preset = (u8)val;
  // load descriptor
  src = pdesc_unpickle(&(pnode->desc), src);
  return src;
}

// pickle/unpickle for param descriptors 
// (struct definition in param_common.h
u8* pdesc_pickle(ParamDesc* pdesc, u8* dst) {
  u8 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    *dst = pdesc->label[i];
    ++dst;
  }
  // store unit string
  /* for(i=0; i<PARAM_UNIT_LEN; ++i) { */
  /*   *dst = pdesc->unit[i]; */
  /*   ++dst; */
  /* } */
  // store type
  *dst = pdesc->type;
  ++dst;
  // store min
  dst = pickle_32(pdesc->min, dst);
  // store max
  dst = pickle_32(pdesc->max, dst);
  return dst;
}

const u8* pdesc_unpickle(ParamDesc* pdesc, const u8* src) {
  u32 val;
  u8 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    pdesc->label[i] = *src;
    ++src;
  }

  // print_dbg("\r\n unpickled param label: ");
  // print_dbg(pdesc->label);

  /* // store unit string */
  /* for(i=0; i<PARAM_UNIT_LEN; ++i) { */
  /*   pdesc->unit[i] = *src; */
  /*   ++src; */
  /* } */

  // print_dbg("\r\n unpickled param unit: ");
  // print_dbg(pdesc->unit);

  // store type
  pdesc->type = *src;
  ++src;

  // print_dbg("\r\n unpickled param type: ");
  // print_dbg_ulong((u32)(pdesc->type));
  
  // store min
  src = unpickle_32(src, &val);
  pdesc->min = val;

  // print_dbg("\r\n unpickled param min: ");
  // print_dbg_hex(pdesc->min);

  // store max
  src = unpickle_32(src, &val);
  pdesc->max = val;

  // print_dbg("\r\n unpickled param max: ");
  // print_dbg_hex(pdesc->max);

  return src;
}


// fill buffer with readable value string
/* ???
void get_param_string(char* dst, u32 idx) {
}
*/

// increment value
io_t inc_param_value(u32 idx,  io_t inc) {
  io_t in;
  s32 scaled;

  /* print_dbg("\r\n inc_param_value, index: "); */
  /* print_dbg_ulong(idx); */

  /* print_dbg(" , input: 0x"); */
  /* print_dbg_hex(get_param_value(idx)); */

  /* print_dbg(" , increment: 0x"); */
  /* print_dbg_hex(inc); */


  in = get_param_value(idx);
  // use scaler to increment and lookup
  scaled = scaler_inc( &(net->params[idx].scaler), &in, inc);

  /* print_dbg(" , new input: 0x"); */
  /* print_dbg_hex(in); */

  /* print_dbg(" , scaled: 0x"); */
  /* print_dbg_hex(scaled); */

  /* print_dbg("\r\n\r\n "); */

  // store input value in pnode
  net->params[idx].data.value = in;
  net->params[idx].data.changed = 1;
  ctl_param_change(idx, scaled );  

  return in;
			   
}
