/*
  op_screen

  raw screen drawing operator.

 */

#ifndef _OP_SCREEN_H_
#define _OP_SCREEN_H_

// avr32lib
#include "region.h"
#include "timers.h"

// bees
#include "net_poll.h"
#include "op.h"
#include "op_math.h"
#include "types.h"


//////  FIXME: trying with a full quad to start with
// width and height
#define OP_SCREEN_PX_W 128
#define OP_SCREEN_PX_H 64
#define OP_SCREEN_GFX_BYTES ( OP_SCREEN_PX_W * OP_SCREEN_PX_H )
// max offsets
#define OP_SCREEN_X_MAX ( SCREEN_ROW_PX - 1 )
#define OP_SCREEN_Y_MAX ( SCREEN_COL_PX - 1 )

//--- op_screen_t : numerical graphic operator
typedef struct op_screen_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t enable;
  volatile io_t period;
  volatile io_t val;
  volatile io_t fill;
  volatile io_t x;
  volatile io_t y;
  // pointers for external access
  volatile io_t* in_val[6];
  // no outputs
  op_out_t outs[0];
  // drawing region handler
  region reg;
  // drawing region data.
  /// FIXME: this should probly be alloc'd/freed from heap.
  u8 regData[OP_SCREEN_GFX_BYTES];
  // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;

} op_screen_t;

extern void op_screen_init(void* screen);
extern void op_screen_deinit(void* screen);

#endif // header guard
