/*
  op_bignum

  antialiased number graphic.

 */

#ifndef _OP_BIGNUM_H_
#define _OP_BIGNUM_H_

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
#define OP_BIGNUM_PX_W 64
#define OP_BIGNUM_PX_H 32
#define OP_BIGNUM_GFX_BYTES ( OP_BIGNUM_PX_W * OP_BIGNUM_PX_H )
// max offsets
#define OP_BIGNUM_X_MAX ( SCREEN_ROW_PX - OP_BIGNUM_PX_W ) 
#define OP_BIGNUM_Y_MAX ( SCREEN_COL_PX - OP_BIGNUM_PX_H ) 

//--- op_bignum_t : numerical graphic operator
typedef struct op_bignum_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t enable;
  volatile io_t period;
  volatile io_t val;
  volatile io_t x;
  volatile io_t y;
  // pointers for external access
  volatile io_t* in_val[5];
  // no outputs
  op_out_t outs[0];
  // drawing region handler
  region reg;
  // drawing region data.
  /// FIXME: this should probly be alloc'd/freed from heap...?
  u8 regData[OP_BIGNUM_GFX_BYTES];
  // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;

} op_bignum_t;

extern void op_bignum_init(void* bignum);
extern void op_bignum_deinit(void* bignum);

#endif // header guard
