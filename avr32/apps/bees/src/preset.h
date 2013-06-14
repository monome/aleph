/* preset.h
 * bees
 * aleph
*/

#ifndef _PRESET_H_
#define _PRESET_H_

#include "net.h"

#define PRESET_NAME_LEN 16


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
  char name[PRESET_NAME_LEN];  
} preset_t;

//==============================
//=== vars
//extern preset_t* presets;
extern preset_t* presets;

//========================
//==== functions

// intialize
void preset_init(void);
// de-initialize
void preset_deinit(void);
// store (and enable) a particular input
void preset_store_in(u32 preIdx, u32 inIdx);
// store (and enable) a particular output
void preset_store_out(u32 preIdx, u32 outIdx);
// store everything enabled in given preset
void preset_store(u32 preIdx);
// recall everything enabled in given preset
void preset_recall(u32 preIdx);
// preset name
char* preset_name(u32 id);
////// this is so terrible and dirty
//preset_t** preset_get_presets(void);

#endif
