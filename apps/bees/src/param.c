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

///// preset-enabled is stored by input node
//// network 
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
  s32 scaled;

  print_dbg("\r\n set_param_value, index: ");
  print_dbg_ulong(idx);
  print_dbg(" , value: 0x");
  print_dbg_hex(val);

  scaled = scaler_get_value( &(net->params[idx].scaler), val);

  print_dbg(" , scaled: 0x");
  print_dbg_hex(scaled);
    
  // network data holds linear input value
  net->params[idx].data.value = val;
  // "changed" flag isn't really used but keep it updated for now
  net->params[idx].data.changed = 1;

  // set scaled
  ctl_param_change(idx, scaled );
}


// pickle / unpickle
u8* param_pickle(pnode_t* pnode, u8* dst) {
  u32 val;
  /// wasting some space to preserve 4-byte alignment
  // store idx


  //// TEST: don't
  //dst = pickle_32((u32)pnode->idx, dst);
  //// TEST: keep as dummy for compatibility
  dst = pickle_32(0, dst);
  ////


  // print_dbg("\r\n pickling param node, value: 0x");
  // print_dbg_hex(pnode->data.value.asUint);

  // store value
  val = (u32)(pnode->data.value);
  dst = pickle_32(val, dst);

  // store play-inclusion 
  dst = pickle_32((u32)(pnode->play), dst);

  // store descriptor
  dst = pdesc_pickle(&(pnode->desc), dst);
  return dst;
}

const u8* param_unpickle(pnode_t* pnode, const u8* src) {
  u32 val;
  // load idx 
  // TEST: keep as dummy for compatibility
  src = unpickle_32(src, &val);

  // print_dbg("\r\n unpickled param index: ");
  // print_dbg_ulong(val);


  ///// TEST: don't
  //  pnode->idx = (u8)val;
  ////
  // load value  
  src = unpickle_32(src, &val);
  pnode->data.value = (ParamValue)val;

  print_dbg("\r\n unpickling param, val: 0x"); 
  print_dbg_hex(val);

  //  print_dbg(" , val: ");
  //  print_dbg_ulong(val);
  //  src = unpickle_32(src, &(pnode->data.value.asUint));

  // print_dbg("\r\n unpickled param value: ");
  // print_dbg_ulong(val);

  // load play-inclusion 
  src = unpickle_32(src, &val);
  pnode->play = (u8)val;


  print_dbg(" , play: 0x"); 
  print_dbg_hex(val);


  // print_dbg("\r\n unpickled param preset flag: ");
  // print_dbg_ulong(val);

  // load descriptor
  src = pdesc_unpickle(&(pnode->desc), src);
  return src;
}

// pickle/unpickle for param descriptors 
// (struct definition in param_common.h
u8* pdesc_pickle(ParamDesc* pdesc, u8* dst) {
  u32 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    *dst = pdesc->label[i];
    ++dst;
  }
  // store type
  // pad for alignment
  // store type (pad for alignment)
  dst = pickle_32((u32)(pdesc->type), dst);
  // store min
  dst = pickle_32(pdesc->min, dst);
  // store max
  dst = pickle_32(pdesc->max, dst);
  // store radix (pad for alignment)
  dst = pickle_32((u32)(pdesc->radix), dst);

  return dst;
}

const u8* pdesc_unpickle(ParamDesc* pdesc, const u8* src) {
  u32 val;
  u32 i;

  print_dbg(", descriptor: ");

  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    pdesc->label[i] = *src;
    ++src;
  }

  print_dbg(" , label: ");
  print_dbg(pdesc->label);

  // store type
  // pad for alignment
  src = unpickle_32(src, &val);
  pdesc->type = (u8)val;
  print_dbg(" \t, type: ");
  print_dbg_ulong(pdesc->type);
  
  // min
  src = unpickle_32(src, &val);
  pdesc->min = val;
  print_dbg(" \t, min: ");
  print_dbg_hex(pdesc->min);

  // max
  src = unpickle_32(src, &val);
  pdesc->max = val;
  print_dbg(" , max: ");
  print_dbg_ulong(pdesc->max);

  // store radix
  // pad for alignment
  src = unpickle_32(src, &val);
  pdesc->radix = (u8)val;

  print_dbg(" , radix: ");
  print_dbg_ulong(pdesc->radix);

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

  print_dbg("\r\n inc_param_value, index: ");
  print_dbg_ulong(idx);

  print_dbg(" , input: 0x");
  print_dbg_hex(get_param_value(idx));

  print_dbg(" , increment: 0x");
  print_dbg_hex(inc);


  in = get_param_value(idx);
  // use scaler to increment and lookup
  scaled = scaler_inc( &(net->params[idx].scaler), &in, inc);

  print_dbg(" , new input: 0x");
  print_dbg_hex(in);

  print_dbg(" , scaled: 0x");
  print_dbg_hex(scaled);

  print_dbg("\r\n\r\n ");

  // store input value in pnode
  net->params[idx].data.value = in;
  net->params[idx].data.changed = 1;
  ctl_param_change(idx, scaled );  

  return in;
			   
}
