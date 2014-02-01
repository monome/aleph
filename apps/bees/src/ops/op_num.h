/*
  op_num

  antialiased number graphic.

 */

#ifndef _OP_NUM_H_
#define _OP_NUM_H_

// avr32lib
#include "region.h"

// bees
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_num_t : numerical graphic operator
typedef struct op_num_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t focus;
  volatile io_t val;
  volatile io_t x;
  volatile io_t y;
  // pointers for external access
  volatile io_t* in_val[4];
  // no outputs
  op_out_t outs[0];
  // drawing region handler
  region reg;
  // drawing region data.
} op_num_t;

extern void op_num_init(void* num);
extern void op_num_deinit(void* num);

#endif // header guard
