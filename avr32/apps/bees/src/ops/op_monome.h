/*
  op_monome.h
  bees
  aleph

  additional abstract superclasses for monome device "layout" operators.

*/

#ifndef _ALEPH_BEES_OP_MONOME_H_
#define _ALEPH_BEES_OP_MONOME_H_

// common
#include "types.h"
// bees
#include "net_monome.h"
#include "op.h"
#include "op_math.h"

// handler function
typedef void(*monome_handler_t)(u32 event);

// generic monome type
// has event handler and 
typedef struct _op_monome_grid {
  // handler function, will connect to app event handler
  monome_handler_t handler;
  // focus flag
  u8 focus;
} op_monome_t;


#endif //h guard
