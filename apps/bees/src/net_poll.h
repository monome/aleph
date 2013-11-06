/*
  net_poll.h
  bees
  aleph
  
  abstract superclass and handling glue for polled operators.

  avr32_lib defines a generic event type for app-specific events.
  here we use it for operators that require a timer callback.

 */

#ifndef _ALEPH_BEES_NET_POLL_H_
#define _ALEPH_BEES_NET_POLL_H_

// aleph-common
#include "types.h"

//-------------------
//---- types

typedef void(*poll_handler_t)(void* op);

// polled operator superclass
typedef struct _op_poll {
// pointer to operator subclass
  void* op; 
  // pointer to handler function
  poll_handler_t handler; 
} op_poll_t;

//--------------------
//--- extern functions

// generic callback
void net_poll_handler(s32 data);

#endif // h guard
