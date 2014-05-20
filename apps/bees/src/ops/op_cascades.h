#ifndef _ALEPH_BEES_OP_CASCADES_H_
#define _ALEPH_BEES_OP_CASCADES_H_

#include "net_monome.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_step : monome grid as a complex cascade counter
typedef struct op_cascades_struct {
  op_t super;
  op_monome_t monome;
  // inputs: mode, focus, step
  volatile io_t focus;
  volatile io_t size;
  volatile io_t* in_val[3];
  // outputs: a,b,c,d
  op_out_t outs[8];
  // internal:

} op_cascades_t;

// init
void op_cascades_init(void* op);
// de-init
void op_cascades_deinit(void* op);

#endif // header guard
