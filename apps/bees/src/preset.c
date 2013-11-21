/* preset.c
 * bees
 * aleph
 *
 * definitions for preset management 
 *

 TODO: operator network hashes

 */

//#include <stdio.h>
// asf
#ifdef ARCH_AVR32
#include "print_funcs.h"
#endif
// bees
#include "net_protected.h"
#include "param.h"
#include "preset.h"
// aleph
#include "memory.h"
#include "simple_string.h"
#include "types.h"



//-------------------------
//----- extern vars

/// array of presets
preset_t* presets;

// last read index
s32 readIdx = -1;
// last write index
s32 writeIdx = -1;

//------------------------------
//---- static func

// append int to char buffer (left justified, no bounds)
/// very fast, for short unsigned values!

/// FIXME: put in utils.h or something
static inline char* atoi_idx(char* dst, u16 val) {
  u16 dig = 0;
  u16 rem = 0;
  char* pline = dst;
  // 3 digits only 
  if(val > 999) { rem = 999; } else { rem = val; }

  dig = val / 100U;
  rem -= dig * 100U;
  *pline = '0' + dig;
  ++pline;

  dig = rem / 10U;
  rem -= dig * 10U;
  *pline = '0' + dig;
  ++pline;

  *pline = '0' + rem;
   ++pline;    
   return pline;
}


//---------------------
//---- extern funcs
// initialize
void presets_init(void) {
  u32 i, j;
  char* p;

  presets = (preset_t*)alloc_mem(NET_PRESETS_MAX * sizeof(preset_t));
  
  for(i=0; i<NET_PRESETS_MAX; i++) {

    // empty name
    for(j=0; j<PRESET_NAME_LEN; j++) {
      presets[i].name[j] = '\0';
    }

    p = presets[i].name;
    p = atoi_idx(p, i);
    *p = '_';

    // empty inputs
    for(j=0; j<NET_INS_MAX; ++j) {
      presets[i].ins[j].idx = -1;
      presets[i].ins[j].value = 0;
      presets[i].ins[j].enabled = 0;
    }
    // empty outputs
    for(j=0; j<NET_OUTS_MAX; ++j) {
      presets[i].outs[j].target = -1;
      presets[i].outs[j].outIdx = -1;
      presets[i].outs[j].enabled = 0;
    }

    // empty params
    for(j=0; j<NET_PARAMS_MAX; ++j) {
      presets[i].params[j].idx = -1;
      presets[i].params[j].value = 0;
      presets[i].params[j].enabled = 0;
    } 
  }
}

// de-initialize
void presets_deinit(void) {
}

// store a particular input
void preset_store_in(u32 preIdx, u32 inIdx) {  
  presets[preIdx].ins[inIdx].enabled = net_get_in_preset(inIdx);
  presets[preIdx].ins[inIdx].value = net_get_in_value(inIdx);
}

// store a particular output
void preset_store_out(u32 preIdx, u32 outIdx) {
  presets[preIdx].outs[outIdx].enabled = net_get_out_preset(outIdx);
  presets[preIdx].outs[outIdx].target = net_get_target(outIdx);  
}

// store everything enabled in given preset
void preset_store(u32 preIdx) {
  u16 i;
  // ins
  for(i=0; i<net_num_ins(); ++i) {
    if( net_get_in_preset(i) ) {
      presets[preIdx].ins[i].value = net_get_in_value(i);
      presets[preIdx].ins[i].enabled = 1;
    }
  }
  // outs
  for(i=0; i<net_num_outs(); ++i) {
    if(net_get_out_preset(i)) {
      presets[preIdx].outs[i].target = net_get_target(i);
      presets[preIdx].outs[i].enabled = 1;
    }
  }
  // params
  for(i=0; i<net_num_params(); ++i) {
    if(get_param_preset(i)) {
      presets[preIdx].params[i].value =  get_param_value( i );
      presets[preIdx].params[i].enabled = 1;
    }
  }
}

// recall everything enabled in given preset
void preset_recall(u32 preIdx) {
  u16 i;
  print_dbg("\r\n preset_recall INS");
  // ins
  for(i=0; i<net_num_ins(); ++i) {
    if(presets[preIdx].ins[i].enabled) {
      print_dbg("\r\n recalling enabled input in target preset, idx: ");
      print_dbg_ulong(i);
      net_set_in_value( i, presets[preIdx].ins[i].value );
    }
  }

  print_dbg("\r\n preset_recall OUTS");
  // outs
  for(i=0; i<net_num_outs(); ++i) {
    if(presets[preIdx].outs[i].enabled) {

      print_dbg("\r\n recalling enabled input in target preset, idx: ");
      print_dbg_ulong(i);
      net_connect( i, presets[preIdx].outs[i].target );
    }
  }

  print_dbg("\r\n preset_recall PARAMS");
  // params
  for(i=0; i<net_num_params(); ++i) {
    if(presets[preIdx].params[i].enabled) {

      print_dbg("\r\n recalling enabled input in target preset, idx: ");
      print_dbg_ulong(i);

      set_param_value( i, presets[preIdx].params[i].value );
    }
  }
  readIdx = preIdx;
}

// preset name
char* preset_name(u32 id) {
  //  print_dbg("\r\n request for preset name at address: ");
  //  print_dbg_hex((u32)&(presets[id]));
  return presets[id].name;
}

// pickle presets
u8* presets_pickle(u8* dst) {  
  u32 i, j;
  for(i=0; i<NET_PRESETS_MAX; i++) {
    // pickle inputs
    for(j=0; j<NET_INS_MAX; j++) {
      // waste some space for 4-byte alignment
      dst = pickle_32(presets[i].ins[j].value, dst);
      dst = pickle_32(presets[i].ins[j].idx, dst);
      dst = pickle_32(presets[i].ins[j].enabled, dst);
    }
    // pickle outpu
    for(j=0; j<NET_OUTS_MAX; j++) {
      // waste some space for 4-byte alignment
      dst = pickle_32(presets[i].outs[j].target, dst);
      dst = pickle_32(presets[i].outs[j].outIdx, dst);
      dst = pickle_32(presets[i].outs[j].enabled, dst);
    }
    // pickle params
    for(j=0; j<net_PARAMS_MAX(); j++) {
      // waste some space for 4-byte alignment
      dst = pickle_32(presets[i].params[j].value, dst);
      dst = pickle_32(presets[i].params[j].idx, dst);
      dst = pickle_32(presets[i].params[j].enabled, dst);
    }
  }
  return dst;  
}

// unpickle
const u8* presets_unpickle(const u8* src) {
  u32 i, j;
  u32 v32;
  for(i=0; i<NET_PRESETS_MAX; i++) {
    // pickle inputs
    for(j=0; j<NET_INS_MAX; j++) {
      // waste some space for 4-byte alignment
      src = unpickle_32(src, &v32);
      presets[i].ins[j].value = (io_t)v32;
      src = unpickle_32(src, &v32);
      presets[i].ins[j].idx = v32;
      src = unpickle_32(src, &v32);
      presets[i].ins[j].enabled = v32;
    }
    // unpickle outputs
    for(j=0; j<NET_OUTS_MAX; j++) {
      // waste some space for 4-byte alignment
      src = unpickle_32(src, &v32);
      presets[i].outs[j].target = (io_t)v32;
      src = unpickle_32(src, &v32);
      presets[i].outs[j].outIdx = v32;
      src = unpickle_32(src, &v32);
      presets[i].outs[j].enabled = v32;
    }
    // unpickle params
    for(j=0; j<NET_PARAMS_MAX; j++) {
      // waste some space for 4-byte alignment
      src = unpickle_32(src, &v32);
      presets[i].params[j].value = (io_t)v32;
      src = unpickle_32(src, &v32);
      presets[i].params[j].idx = v32;
      src = unpickle_32(src, &v32);
      presets[i].params[j].enabled = v32;
    }
  }
  return src;
}

// get last read index
s32 preset_last_read(void) {
  return readIdx;
}

// get last write index
s32 preset_last_write(void) {
  return writeIdx;
}
