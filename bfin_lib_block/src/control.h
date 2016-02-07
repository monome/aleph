/**
 * control.h
 * 
 * process parameter changes
 */

#ifndef _ALEPH_BFIN_CONTROL_H_
#define _ALEPH_BFIN_CONTROL_H_

#include "param_common.h"
#include "types.h"


//! keep track of cycles spent processing each param change block
// extern volatile u64 controlCycleCount;

//! add a parameter change to the queue
//! should be called with interrupts disabled (e.g. from an ISR)
extern void control_add(u32 idx, ParamValue val);

//! process pending param changes.
//! should be called with interrupts disabled (e.g. from an ISR)
extern void control_process(void);

#endif
