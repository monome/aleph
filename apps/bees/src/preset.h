/* preset.h
 * bees
 * aleph
*/

#ifndef _PRESET_H_
#define _PRESET_H_

#include "net.h"
#include "op_math.h"

#define PRESET_NAME_LEN 16
#define PRESET_INODES_COUNT (NET_INS_MAX + NET_PARAMS_MAX)

//=================================
//===== types

// minimal storage for input
typedef struct _presetIn {
  io_t value;
  // FIXME: we would need to store idx for conditional inclusion in scene binary.
  //  s16 idx;
  u8 enabled;
} presetIn_t;


// minimal storage for output
typedef struct _presetOut {
  s16 target;
  // FIXME: we would need to store idx for conditional inclusion in scene binary.
  //  s16 outIdx;
  u8 enabled;
} presetOut_t;


/* // minimal storage for parameter */
/* typedef struct _presetParam { */
/*   io_t value; */
/*   s16 idx; */
/*   u8 enabled; */
/* } presetParam_t; */

// preset structure
typedef struct _preset {
  presetIn_t ins[PRESET_INODES_COUNT];
  presetOut_t outs[NET_OUTS_MAX];
  //  presetParam_t params[NET_PARAMS_MAX];
  char name[PRESET_NAME_LEN];  
} preset_t;

//==============================
//=== vars
//extern preset_t* presets;
extern preset_t* presets;

//========================
//==== functions

// intialize
extern void presets_init(void);
// de-initialize
extern void presets_deinit(void);
// store (and enable) a particular input
extern void preset_store_in(u32 preIdx, u32 inIdx);
// store (and enable) a particular output
extern void preset_store_out(u32 preIdx, u32 outIdx);
// store everything enabled in given preset
extern void preset_store(u32 preIdx);
// recall everything enabled in given preset
extern void preset_recall(u32 preIdx);
// get preset name
extern char* preset_name(u32 id);
// pickle presets
extern u8* presets_pickle(u8* dst);
// unpickle
extern const u8* presets_unpickle(const u8* src);

// get current read/write selection index
extern s32 preset_get_select(void);
// get pointer to selected preset
extern preset_t* preset_get_selected(void);
// set current read/write selection
extern void preset_set_select(s32 idx);
// increment current selection
extern void preset_inc_select(s32 inc);

//---- get
// get inclusion flag for given input, given preset
extern u8 preset_in_enabled(u32 preIdx, u32 inIdx);
// get inclusion flag for given output, given preset
extern u8 preset_out_enabled(u32 preIdx, u32 inIdx);

// clear a given preset
extern void preset_clear(int idx);




#endif
