#ifndef _op_life_H_
#define _op_life_H_

#include "op.h"
#include "op_math.h"
#include "types.h"
#include "net_monome.h"


//--- op_life_t : conways life
// next
// size x (8/16)
// sixe y (8/16)
// x
// y
// set (0/1)
// noise (0-100 likelihood)
// rules (0 = normal, 1 = nodeath, 2 = nobirth)

// pop
// delta
// average (over 16 stages)
// dense point x/y
// val @ in x/y


typedef struct op_life_struct {
  op_t super;
  op_monome_t monome; 

  volatile u16 pop, lpop;

  volatile io_t next;
  volatile io_t xsize;
  volatile io_t ysize;
  volatile io_t x;
  volatile io_t y;
  volatile io_t set;
  volatile io_t noise;
  volatile io_t rules;
  volatile io_t focus;
  volatile io_t* in_val[9];
  op_out_t outs[3];
} op_life_t;

void op_life_init(void* life);
void op_life_deinit(void* life);

#endif // header guard
