/* preset.c
 * bees
 * aleph
 *
 * definitions for preset management 
 */

#include "../common/types.h"
#include "net_protected.h"
#include "preset.h"

//=================================
//===== types
typedef struct preset_struct {
  inode_t ins[NET_INS_MAX];
  onode_t outs[NET_OUTS_MAX];
} preset_t;

//=================================
//====== variables
static preset_t presets[NUM_PRESETS];

//=================================
//====== function definitions

// initialize
void preset_init(void) {
}

// de-initialize
void preset_deinit(void) {
}

// store (and enable) a particular input
void preset_store_in(U32 preIdx, U32 inIdx) {
}

// store (and enable) a particular output
void preset_store_out(U32 preIdx, U32 outIdx) {
}

// store everything enabled in given preset
void preset_store(U32 preIdx) {
}

// recall everything enabled in given preset
void preset_recall(U32 preIdx) {
}
