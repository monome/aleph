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

// fixme: trying with a full quad to start with
#define OP_NUM_PIX_W 64
#define OP_NUM_PIX_H 32
#define OP_NUM_GFX_BYTES (OP_NUM_PIX_W * OP_NUM_PIX_H)

//--- op_num_t : numerical graphic operator
typedef struct op_num_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t val;
  volatile io_t x;
  volatile io_t y;
  // pointers for external access
  volatile io_t* in_val[3];
  // no points
  op_out_t outs[0];
  // drawing region handler
  region reg;
  // drawing region data.
} op_num_t;

extern void op_num_init(void* num);

#endif // header guard
