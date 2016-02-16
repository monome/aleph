//#include <string.h>
#include "control.h"
#include "module.h"


///// FIXME! 

/*
  there is no buffer or separate task for control changes.
  they are set directly from the SPI ISR!
  very bad.
 */

#if 0

// ordered set of param changes
static ctlEvent_t ctlBuf[CTL_BUF_SIZE];
// dirty flags
static u8 paramsDirty[DIRTY_BYTES];
// count of param change events
static s32 evCount = 0;
// audio frames since last update
static u32 frameCount = 0;

// get dirty flag for param idx (inlined in header)
//extern u8 get_param_dirty(u32 idx);

static inline u8 get_param_dirty(u32 idx) {
  //return BITTEST(paramsDirty, idx);
  return paramsDirty[idx] > 0;
}
static void set_param_dirty(u32 idx) {
  //      BITSET(paramsDirty, idx);
  paramsDirty[idx] = 1;

}
// clear dirty flag
static void clear_param_dirty(u32 idx) {
  paramsDirty[idx] = 0;
}
// clear all dirty flags
/* static void clear_all_param_dirty(void) { */
/*   memset(paramsDirty, 0, NUM_PARAMS); */
/* } */

// request a parameter change.
extern u8 ctl_param_change(u32 idx, u32 val) {
  u32 i;
  if(get_param_dirty(idx)) {
    // search for event with this param idx
    for(i=0; i<evCount; i++) {
      if(ctlBuf[i].idx == idx) {
	ctlBuf[i].val = (ParamValue)val;
	break;
      }
    }
    return CTL_REQUEST_OK;
  } else {
    // add a new event
    if(evCount < CTL_BUF_SIZE_1) {
      ctlBuf[evCount].idx = idx;
      ctlBuf[evCount].val = (ParamValue)val;
      evCount++; 
      // update dirty flag
      set_param_dirty(idx);
      return 0;
    } else {
      return 1;
    }
  }
}

// execute the last param change
extern void ctl_do_last_change(void) {
  static u32 idx;
  idx = ctlBuf[evCount - 1].idx;
  module_set_param(idx, ctlBuf[evCount - 1].val);
  clear_param_dirty(idx);
  evCount--;
}



// execute the last param change
//// warning: no lower bounds check on evCount
extern void ctl_perform_last_change(void) {
  static s32 idx;
  if(evCount == 0) { return; }
  idx = ctlBuf[--evCount].idx;
  module_set_param(idx, ctlBuf[evCount].val);
  clear_param_dirty(idx);
}

// attempt to execute all pending param changes
extern void ctl_perform_all_changes(void) {
  while(evCount > 0) {
    ctl_perform_last_change();
  }
}

// tick every audio frame
extern void ctl_next_frame(void) {
  frameCount++;
  if(frameCount == CTL_BUF_SIZE) {
    frameCount = 0;
  }
  if(evCount > 0) {
    ctl_do_last_change();
  }
}

#endif
