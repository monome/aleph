#ifndef _ALEPH_BEES_OP_HID_WORD_H_
#define _ALEPH_BEES_OP_HID_WORD_H_

#include "net_hid.h"

//--- op_hid_word_t : HID data, variable size
typedef struct op_hid_word_struct {
  // operator base class
  op_t super;
  // hid operator abstract class
  op_hid_t hid;  
  // inputs: byte index, word size
  volatile io_t* in_val[2];
  // outputs: value
  op_out_t outs[1];
  // index of first data byte in frame
  volatile io_t byte;
  // word size
  volatile io_t size;
} op_hid_word_t;

// init
void op_hid_word_init(void* op);
// de-init
void op_hid_word_deinit(void* op);

#endif // h guard
