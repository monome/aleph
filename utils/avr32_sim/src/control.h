
/*
  control.h
  avr32
  aleph
 
  FIXME:  
  duplicating param change queue from the blackfin.
  this is decidedly sub-optimal but lets get things working.

  parameter changes are stored in a set structure.
  maximum one request per parameter enters the queue
  
 */

#ifndef _ALEPH_BFIN_CONTROL_H_
#define _ALEPH_BFIN_CONTROL_H_

//#include "module_custom.h"
#include "param_common.h"
#include "types.h"

//---------------------------------------------
//---- definitions / types

// add param change to buffer
/// FIXME: uh will this work for params < 0 ?
extern u8 ctl_param_change(u32 param, u32 value);

#endif // h guard
