#ifndef _op_life_raw_H_
#define _op_life_raw_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_life_raw_t : conways life
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


typedef struct op_life_raw_struct {
  op_t super;

  volatile u16 pop, lpop;

  volatile io_t next;
  volatile io_t xsize;
  volatile io_t ysize;
  volatile io_t x;
  volatile io_t y;
  volatile io_t write;
  volatile io_t read;
  volatile io_t tog;
  volatile io_t rules;
  volatile io_t* in_val[9];
  op_out_t outs[4];
  s8 lifenow[16][16];
  s8 lifenext[16][16];
} op_life_raw_t;

void op_life_raw_init(void* life);
void op_life_raw_deinit(void* life);

#endif // header guard
