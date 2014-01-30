/*
  net_poll.c
  bees
  aleph
  
  abstract superclass for polled operators.

 */

#include "print_funcs.h"

#include "net_poll.h"

// timed event handler
void net_poll_handler(s32 data) {
  // print_dbg("\r\n bees, net_poll_handler, data: 0x");
  // print_dbg_hex((u32)data);
  
  // cast from data to pointer.. is this a bad idea?
  op_poll_t* op_poll = (op_poll_t*)data;
  (*(op_poll->handler))(op_poll->op);
}
