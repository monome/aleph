
#ifndef _OP_BARS8_H_
#define _OP_BARS8_H_

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
#define OP_BARS8_PX_W 128
#define OP_BARS8_PX_H 64
#define OP_BARS8_GFX_BYTES ( OP_BARS8_PX_W * OP_BARS8_PX_H )
// max offsets
#define OP_BARS8_X_MAX ( SCREEN_ROW_PX - 1 )
#define OP_BARS8_Y_MAX ( SCREEN_COL_PX - 1 )

//--- op_screen_t : numerical graphic operator
typedef struct op_bars8_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t enable;
  volatile io_t period;
  volatile io_t mode;
  volatile io_t a;
  volatile io_t b;
  volatile io_t c;
  volatile io_t d;
  volatile io_t e;
  volatile io_t f;
  volatile io_t g;
  volatile io_t h;
  // pointers for external access
  volatile io_t* in_val[11];
  // no outputs
  op_out_t outs[0];
  // drawing region handler
  region reg;
  // drawing region data.
  /// FIXME: this should probly be alloc'd/freed from heap.
  u8 regData[OP_BARS8_GFX_BYTES];
  // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;

} op_bars8_t;

extern void op_bars8_init(void* bars8);
extern void op_bars8_deinit(void* bars8);

#endif // header guard
