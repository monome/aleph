/* preset.c
 * bees
 * aleph
 *
 * definitions for preset management 
 *

 TODO: operator network hashes

 */



#include "types.h"
#include "net_protected.h"
#include "param.h"
#include "preset.h"

//=================================
//===== types

// minimal storage for input
typedef struct _presetIn {
  f32 value;
  u16 idx;
  u8 enabled;
} presetIn_t;


// minimal storage for output
typedef struct _presetOut {
  s16 target;
  u16 outIdx;
  u8 enabled;
} presetOut_t;


// minimal storage for parameter
typedef struct _presetParam {
  f32 value;
  u16 idx;
  u8 enabled;
} presetParam_t;

// preset structure
typedef struct _preset {
  presetIn_t ins[NET_INS_MAX];
  presetOut_t outs[NET_OUTS_MAX];
  presetParam_t params[NET_PARAMS_MAX];
} preset_t;

//=================================
//====== variables

/// aarray of presets
static preset_t presets[NET_PRESETS_MAX];

//=================================
//====== function definitions

// initialize
void preset_init(void) {
}

// de-initialize
void preset_deinit(void) {
}

// store a particular input
void preset_store_in(U32 preIdx, U32 inIdx) {  
  presets[preIdx].ins[inIdx].enabled = net_get_in_preset(inIdx);
  presets[preIdx].ins[inIdx].value = net_get_in_value(inIdx);
}

// store a particular output
void preset_store_out(U32 preIdx, U32 outIdx) {
  presets[preIdx].outs[outIdx].enabled = net_get_out_preset(outIdx);
  presets[preIdx].outs[outIdx].target = net_get_target(outIdx);  
}

// store everything enabled in given preset
void preset_store(U32 preIdx) {
  u16 i;
  // ins
  for(i=0; i<NET_INS_MAX; i++) {
    if( net_get_in_preset(i) ) {
      presets[preIdx].ins[i].value = net_get_in_value(i);
      presets[preIdx].ins[i].enabled = 1;
    }
  }
  // outs
  for(i=0; i<NET_OUTS_MAX; i++) {
    if(net_get_out_preset(i)) {
      presets[preIdx].outs[i].target = net_get_target(i);
      presets[preIdx].outs[i].enabled = 1;
    }
  }
  // params
  for(i=0; i<NET_PARAMS_MAX; i++) {
    if(get_param_preset(i)) {
      presets[preIdx].params[i].value =  get_param_value( i );
      presets[preIdx].params[i].enabled = 1;
    }
  }
}

// recall everything enabled in given preset
void preset_recall(U32 preIdx) {
  u16 i;
  // ins
  for(i=0; i<NET_INS_MAX; i++) {
    if(presets[preIdx].ins[i].enabled) {
      net_set_in_value( i, presets[preIdx].ins[i].value );
    }
  }
  // outs
  for(i=0; i<NET_OUTS_MAX; i++) {
    if(presets[preIdx].outs[i].enabled) {
      net_connect( i, presets[preIdx].outs[i].target );
    }
  }
  // params
  for(i=0; i<NET_PARAMS_MAX; i++) {
    if(presets[preIdx].params[i].enabled) {
      set_param_value( i, presets[preIdx].params[i].value );
    }
  }
}
