#include <string.h>

#include "print_funcs.h"

#include "bfin.h"
#include "control.h"
//#include "module.h"

// ordered set of param changes
static ctlEvent_t ctlBuf[CTL_BUF_SIZE];
// dirty flags
#if 0
static u8 paramsDirty[DIRTY_BYTES];
#endif
// count of param change events
static u32 evCount = 0;
// audio frames since last update

// get dirty flag for param idx (inlined in header)
//extern u8 get_param_dirty(u32 idx);

#if 0
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
#endif
// clear all dirty flags
/* static void clear_all_param_dirty(void) { */
/*   memset(paramsDirty, 0, NUM_PARAMS); */
/* } */


// request a parameter change.
extern u8 ctl_param_change(u32 idx, u32 val) {
#if 1 // testing: direct set
  //  print_dbg("\r\n set: ");
  bfin_set_param(idx, val);
  return 0;
#else
  u32 i;
  if(get_param_dirty(idx)) {
    // search for event with this param idx
    for(i=0; i<evCount; i++) {
      if(ctlBuf[i].idx == idx) {
	ctlBuf[i].val.u = val;
	break;
      }
    }
    return CTL_REQUEST_OK;
  } else {
    // add a new event
    if(evCount < CTL_BUF_SIZE_1) {
      ctlBuf[evCount].idx = idx;
      ctlBuf[evCount].val.u = val;
      evCount++; 
      // update dirty flag
      set_param_dirty(idx);
      return 0;
    } else {
      return 1;
    }
  }
#endif
}

// execute the most recent change
//// warning: no lower bounds check on evCount
extern void ctl_perform_last_change(void) {
  static u32 idx;
  idx = ctlBuf[--evCount].idx;
  print_dbg("\r\n performing control change, decremented count is now 0x");
  print_dbg_hex((u32)evCount);
  bfin_set_param(idx, ctlBuf[evCount].val.fix);
  #if 0
  clear_param_dirty(idx);
  #endif
}


// attempt to execute all pending param changes
extern void ctl_perform_all_changes(void) {
  u32 idx, i;
  // execute in FIFO order
  for(i=0; i<evCount; i++) {
    idx = ctlBuf[i].idx;
    print_dbg("\r\n performing control ");
    bfin_set_param(idx, ctlBuf[i].val.fix);
    #if 0
    clear_param_dirty(idx);
    #endif
  }
  evCount = 0;
}
