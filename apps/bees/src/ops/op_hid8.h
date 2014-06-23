#ifndef _ALEPH_BEES_OP_HID8_H_
#define _ALEPH_BEES_OP_HID8_H_

#include "net_hid.h"

//--- op_hid8_t : single-byte HID data


typedef struct op_hid8_struct {
  // operator base class
  op_t super;
  // hid operator abstract class
  op_hid_t hid;  
  // input: channel, control num
  volatile io_t* in_val[2];
  // outputs: value
  op_out_t outs[1];
  // index of data byte in frame
  volatile io_t byte;
} op_hid8_t;

// init
void op_hid8_init(void* op);
// de-init
void op_hid8_deinit(void* op);

#endif // h guard
