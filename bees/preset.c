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

static u32 curPreset = 0;

//=================================
//====== function definitions

// initialize
void preset_init(void) {
}

// de-initialize
void preset_deinit(void) {
}
