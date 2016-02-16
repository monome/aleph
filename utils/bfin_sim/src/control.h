
/*
  control.h
  blackfin
  aleph
  
  define a control rate for the blackfin.
  parameter changes are stored in a set structure.
  dirty flags for each parameter are stored in a bit array,
  so that only the most recent value of a given parameter can be processed.
  
 */

#ifndef _ALEPH_BFIN_CONTROL_H_
#define _ALEPH_BFIN_CONTROL_H_

#include "module_custom.h"
#include "param_common.h"
#include "types.h"

//---------------------------------------------
//---- definitions / types

// depth of FIFO
#define CTL_BUF_SIZE 32
#define CTL_BUF_SIZE_1 31
// process at most 1 control change per audio sample
// so keep (buf size < (samplerate / control rate))
// frame divisor (sr / kr)
#define CTL_FRAME_DIV 48

// byes in the dirty-flag bitfield
//#define DIRTY_BYTES (BITNSLOTS(NUM_PARAMS))
#define DIRTY_BYTES NUM_PARAMS

//-- return values
// added request
#define CTL_REQUEST_OK   0
// buffer was full
#define CTL_REQUEST_FULL 1

// parameter change event type
typedef struct _ctlEvent {
  u8 idx;
  ParamValue val;
} ctlEvent_t;

//---------------------------------------------
//---- variables


//-----------------------
//----- functions

/* // check dirty flag */
/* inline u8 get_param_dirty(u32 idx) { */
/*   return BITTEST(paramsDirty, idx); */
/* } */


// add param change to buffer
extern u8 ctl_param_change(u32 param, u32 value);
// execute the last param change
extern void ctl_perform_last_change(void);
// attempt to execute all pending param changes
extern void ctl_perform_all_changes(void);
// tick every audio frame
extern void ctl_next_frame(void);

#endif // h guard
