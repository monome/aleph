/* preset.h
 * bees
 * aleph
*/

#ifndef _PRESET_H_
#define _PRESET_H_

#include "net.h"
#include "op_math.h"

#define PRESET_NAME_LEN 16

//=================================
//===== types

// minimal storage for input
typedef struct _presetIn {
  io_t value;
  s16 idx;
  u8 enabled;
} presetIn_t;


// minimal storage for output
typedef struct _presetOut {
  s16 target;
  s16 outIdx;
  u8 enabled;
} presetOut_t;


// minimal storage for parameter
typedef struct _presetParam {
  io_t value;
  s16 idx;
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
void presets_init(void);
// de-initialize
void presets_deinit(void);
// store (and enable) a particular input
void preset_store_in(u32 preIdx, u32 inIdx);
// store (and enable) a particular output
void preset_store_out(u32 preIdx, u32 outIdx);
// store everything enabled in given preset
void preset_store(u32 preIdx);
// recall everything enabled in given preset
void preset_recall(u32 preIdx);
// get preset name
char* preset_name(u32 id);
// pickle presets
u8* presets_pickle(u8* dst);
// unpickle
const u8* presets_unpickle(const u8* src);

#endif
