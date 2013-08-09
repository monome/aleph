/*
  net_poll.h
  bees
  aleph
  
  the network needs to keep a list of registered operators that require a tick.
  tickable operators should inherit from op_poll_t.
  this superclass contains a pointer to the next ticked operator, 
  so thhe network can maintain a linked list without allocating additional memory.

 */

#ifndef _ALEPH_BEES_NET_POLL_H_
#define _ALEPH_BEES_NET_POLL_H_

// aleph-common
#include "types.h"

//-------------------
//---- types

// pollable operator superclass
typedef struct _op_poll {
  struct _op_poll* next;
  struct _op_poll* prev;
  void* op; // pointer to operator subclass
} op_poll_t;

// linked list of pointers
typedef struct _op_list {
  void* top;
  void* cur;
  u32 num;
} op_list_t;


//-------------------------
//--- extern functions
extern void op_list_init(op_list_t* list);

// add op pointer after the current positio
extern void net_poll_list_add(op_list_t* list, op_poll_t* op);
// delete op pointer at current position, return next pointer in list
extern op_poll_t* net_poll_list_remove(op_list_t* list, op_poll_t* op);

#endif // h guard
